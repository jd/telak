/*
 * telak - A program that display pictures in root window
 * (c) 2005, 2006 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 */

/* C stuff */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* gcrypt stuff */
#include <gcrypt.h>

#include "telak.h"
#include "image.h"
#include "fetch.h"

extern struct deskitem *head;
extern struct config conf;
extern Display *disp;
extern Window win;


void
display(struct deskitem *di)
{
  if(!di->loaded)
	return;

  imlib_context_set_image(di->image);
  
  imlib_render_image_on_drawable_at_size(di->x,
										 di->y,
										 di->w,
										 di->h);
}

void
refresh(int signal __attribute__ ((unused)))
{
  load_img(head);
  display(head);
}

void
draw()
{
  struct deskitem *probe;
  XEvent ev;
  struct timeval tv_current;

  /* Init clocks */
  probe = head;
  do
  {
	gettimeofday(&(probe->tv), NULL);
	probe = probe->next;
  }
  while(probe != NULL);

  /* Display everything ! */
  for(probe = head; probe != NULL; probe = probe->next)
	display(probe);

  /* Main loop, should never end */
  while(1)
  {
	/* X events ? */
	while(XPending(disp))
	{
	  XNextEvent(disp, &ev);
	  
	  if(ev.type == Expose)
		for(probe = head; probe != NULL; probe = probe->next)
		  display(probe);
	}

	/* What time is it ? */
	if(gettimeofday(&tv_current, NULL))
	{
	  perror("Error while getting time of day. Buy a clock.");
	  exit(EXIT_FAILURE);
	}

	/* Check timers */
	probe = head;
	do
	{
	  if(probe->refresh &&
	     tv_current.tv_sec - probe->tv.tv_sec >= probe->refresh)
	  {
		load_img(probe);
		
		display(probe);

		gettimeofday(&(probe->tv), NULL);

	  }
	  probe = probe->next;
	}
	while(probe != NULL);
	
	/* Sleep a little ZzzZZzz */
	usleep(1000);
  }
}

int
load(struct deskitem *img)
{
  struct deskitem * probe;

  /* if there isn't a linked list yet, start one */
  if (head == NULL)
  {
	head = (struct deskitem *) malloc(sizeof(struct deskitem));
	
	if(head == NULL)
	{
	  perror("Cannot allocate memory!");
	  exit(EXIT_FAILURE);
	}
	
	probe = head;
	probe->next = NULL;
  }
  else
  {
	/* find the end of the linked list and add a new deskitem */
	probe = head;
	  
	while (probe->next != NULL)
	  probe = probe->next;
	
	probe->next = (struct deskitem *) malloc(sizeof(struct deskitem));
	  
	if(probe->next == NULL)
	{
	  perror("Cannot allocate memory!");
	  exit(EXIT_FAILURE);
	}
	
	probe = probe->next;
	probe->next = NULL;
  }
  
  probe->url = strdup(img->url);
  probe->file = NULL;
  probe->image = NULL;
  probe->loaded = 0;

  /* set reverse */
  probe->reverse = img->reverse;

  /* Load img->file */
  load_img(probe);

  probe->w = img->w;
  probe->h = img->h;

  if(probe->loaded)
  {
	/* if no size is given, use default one */
	if(!probe->h)
	  probe->h = imlib_image_get_height();
	
	if(!probe->w)
	  probe->w = imlib_image_get_width();
  }
  
  /* set the x,y location*/
  probe->x = img->x;
  probe->y = img->y;

  /* set refresh time */
  probe->refresh = img->refresh;

  return 0;
}

int
load_img(struct deskitem *img)
{
  unsigned char * buf;
  unsigned int i, hash_size;
  char * md5;
  char tmp[3];
  struct stat st;
  DATA8 r_table[256];
  DATA8 g_table[256];
  DATA8 b_table[256];
  DATA8 a_table[256];
  Imlib_Color_Modifier color_mod;
  
  if(img->url[0] != '/')
  {
	if(!img->file)
	{
	  hash_size = gcry_md_get_algo_dlen(GCRY_MD_MD5);
	  
	  /* Compute md5 hash of url */
	  buf = (unsigned char *) malloc(hash_size * sizeof(unsigned char));
	  md5 = (char *) malloc(sizeof(char) * (hash_size * 2 + 1));
	  
	  gcry_md_hash_buffer(GCRY_MD_MD5, buf, (char *) img->url, strlen(img->url));
	  
	  for(i = 0 ; i < hash_size ; i++)
	  {
		snprintf(tmp, 3, "%.2x", buf[i]);
		md5[i*2] = tmp[0];
		md5[i*2+1] = tmp[1];
	  }
	  md5[hash_size*2] = '\0';
	  
	  img->file = (char *) malloc(sizeof(char) *
				      (strlen(conf.cache_dir) + strlen(md5) + 2));
	  strcpy(img->file, conf.cache_dir);
	  strcat(img->file, "/");
	  strcat(img->file, md5);
	}
	
	fetch(img->url, img->file);
  }
  else
	img->file = strdup(img->url);
  
  
  /* Check if the file is openable */
  if(stat(img->file, &st))
  {
	fprintf(stderr, "%s ", img->file);
	perror("unable to stat() file");
	return 1;
  }
  
  /* We do not load weird things with imlib */
  if(!S_ISREG(st.st_mode))
  {
	fprintf(stderr, "%s is not a regular file.\n", img->file);
	return 1;
  }

  /* Unload if already loaded */
  if(img->loaded)
  {
	imlib_context_set_image(img->image);
	imlib_free_image_and_decache();
  }

  /* Now really load the file with imlib */
  img->image = imlib_load_image(img->file);
  
  if(img->image)
  {
	imlib_context_set_image(img->image);
	img->loaded = 1;

	if(img->reverse)
	{
	  color_mod = imlib_create_color_modifier();
	  imlib_context_set_color_modifier(color_mod);
	  imlib_reset_color_modifier();
	  
	  imlib_get_color_modifier_tables(r_table, g_table, b_table, a_table);
	  
	  for(i = 0; i <= 255; i++)
	  {
		r_table[255-i] = i;
		g_table[255-i] = i;
		b_table[255-i] = i;
	  }
	  
	  imlib_set_color_modifier_tables(r_table, g_table, b_table, a_table);
	  
	  imlib_apply_color_modifier();
	  
	  imlib_free_color_modifier();
	}
	
	return 0;
  }
  
  return 1;
}

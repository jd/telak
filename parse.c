/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>

#include "telak.h"
#include "image.h"

#define BUF_SIZE 4096

extern struct config conf;

struct deskitem *
init_deskitem(struct deskitem * img)
{
  img->url = NULL;
  img->x = 0;
  img->y = 0;
  img->w = 0;
  img->h = 0;
  img->loaded = 0;
  img->refresh = 0;
  img->reverse = 0;
  
  return img;
}

void
parse()
{
  FILE *rcfile;
  char buf[BUF_SIZE];
  struct deskitem img;
  int new = 0;
  
  init_deskitem(&img);

  /* Now parse rcfile */
  rcfile = fopen(conf.file, "r");
  
  if(!rcfile)
  {
	perror("Error opening configuration file");
	exit(EXIT_FAILURE);
  }

  while(fgets(buf, BUF_SIZE, rcfile))
  {
	/* Skip comments */
	if(buf[0] == '#')
	  continue;

	/* New entry */
	if(buf[0] == '[')
	{
	  printf("Parsing %s", buf);
	  
	  /* Load img in list */
	  if(new && img.url)
		load(&img);
	  else if(new && !img.url)
	  {
		fprintf(stderr, "Error: at least one of your pictures does not have an URL.\n");
		exit(EXIT_FAILURE);
	  }
	  else
		new = 1;
	  
	  /* Reinit img */
	  init_deskitem(&img);
	}
	
	if(!strncmp(buf, "url", 3) && new)
	{
	  img.url = (char *) malloc(sizeof(char) * strlen(buf));
	  sscanf(buf, "%*s = %s", img.url);
	  continue;
	}
	
	if(!strncmp(buf, "height", 6) && new)
	{
	  sscanf(buf, "%*s = %d", &img.h);
	  continue;
	}
	
	if(!strncmp(buf, "width", 5) && new)
	{
	  sscanf(buf, "%*s = %d", &img.w);
	  continue;
	}
	
	if(!strncmp(buf, "x", 1) && new)
	{
	  sscanf(buf, "%*s = %d", &img.x);
	  continue;
	}

	if(!strncmp(buf, "y", 1) && new)
	{
	  sscanf(buf, "%*s = %d", &img.y);
	  continue;
	}

	if(!strncmp(buf, "refresh", 7) && new)
	{
	  sscanf(buf, "%*s = %d", &img.refresh);
	  continue;
	}

	if(!strncmp(buf, "reverse", 7) && new)
	{
	  sscanf(buf, "%*s = %d", &img.reverse);
	  continue;
	}
  }
  
  fclose(rcfile);

  /* Load last img in list */
  if(new && img.url)
	load(&img);
  else if(new && !img.url)
  {
	fprintf(stderr, "Error: at least one of your pictures does not have an URL.\n");
	exit(EXIT_FAILURE);
  }
  
  if(!new)
  {
	fprintf(stderr, "Fatal error: config file is not valid.\n");
	exit(EXIT_FAILURE);
  }

}

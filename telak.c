/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <X11/Xatom.h>

#include "telak.h"
#include "parse.h"
#include "image.h"
#include "toon.h"

extern char *optarg;
extern int optind, opterr, optopt;

/* Global stuff */
struct deskitem *head;
struct config conf;
Display *disp;
Window win;
Visual *vis;
Colormap cm;

int
main(int argc, char **argv)
{
  /* Init configuration */
  conf.display = NULL;
  conf.file = NULL;
  conf.cache_dir = NULL;

  /* Init deskitem list */
  head = NULL;

  /* Make conf */
  make_conf(argc, argv);

  init_x();

  check_cache_dir();

  parse();

  draw();

  return EXIT_SUCCESS;
}


void
usage()
{
  printf("Usage: telak [options]\n");
  printf("Options:\n");
  printf(" --display | -d <display>\t\tSpecify display\n");
  printf(" --config | -c <config>\t\t\tSpecify config file\n");
  printf(" --cache <cachedir>\t\t\tSpecify cache directory\n");
  printf(" --help | -h\t\t\t\tPrint this help\n");
  printf(" --version | -v\t\t\t\tPrint telak version\n");

  exit(EXIT_SUCCESS);
}

void
version()
{
  printf("telak %s\n", TELAK_VERSION);
  exit(EXIT_SUCCESS);
}


void
init_x()
{
  Window parent;

  /* open a connection to the X server */
  if(!(disp = XOpenDisplay(conf.display)))
  {
	fprintf(stderr, "Error while opening display.\n");
	exit(EXIT_FAILURE);
  }

  /* get default visual , colormap etc. you could ask imlib2 for what it */
  vis = DefaultVisual(disp, DefaultScreen(disp));
  cm = DefaultColormap(disp, DefaultScreen(disp));

  /* Get root window */
  win = ToonGetRootWindow(disp, DefaultScreen(disp), &parent);
 
  /* We will redraw on exposure */
  XSelectInput(disp, win, ExposureMask);

  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
  imlib_context_set_colormap(cm);
  imlib_context_set_drawable(win);
}

void make_conf(int argc, char **argv)
{
  int optc;
  struct passwd *pass;
  struct option opt[] = {
	{ "display", 1, 0, 'd' },
	{ "config", 1, 0, 'c' },
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'v' },
	{ "cache", 1, 0, 'a' },
	{ 0, 0, 0, 0}
  };

  while((optc = getopt_long(argc, argv, ":d:c:hv", opt, NULL)) != -1)
  {
	switch(optc)
	{
	case 'h':
	  usage();
	  break;

	case 'v':
	  version();
	  break;

	case 'd':
	  conf.display = strdup(optarg);
	  break;

	case 'c':
	  conf.file = strdup(optarg);
	  break;

	case 'a':
	  conf.cache_dir = strdup(optarg);
	  break;

	case '?':
	  fprintf(stderr, "Unknown option: -%c\n", optopt);
	  exit(EXIT_FAILURE);
	  break;

	case ':':
	  fprintf(stderr, "Missing argument.\n");
	  exit(EXIT_FAILURE);
	  break;
	}
  }

  if(conf.cache_dir == NULL)
  {
	/* Build cache dir path */
	pass = getpwuid(getuid());
	
	conf.cache_dir = (char *) malloc(sizeof(char) * 
									 (strlen(pass->pw_dir) + strlen(TELAK_CACHE_DIR) + 3));

	strcpy(conf.cache_dir, pass->pw_dir);
	strcat(conf.cache_dir, "/");
	strcat(conf.cache_dir, TELAK_CACHE_DIR);
	strcat(conf.cache_dir, "/");
  }

  if(!conf.file)
  {
	/* Build rcfile path */
	pass = getpwuid(getuid());
	
	conf.file = (char *) malloc(sizeof(char) *
								 (strlen(pass->pw_dir) + strlen(TELAK_RCFILE) + 2));
	
	strcpy(conf.file, pass->pw_dir);
	strcat(conf.file, "/");
	strcat(conf.file, TELAK_RCFILE);
  }
}


void
check_cache_dir()
{
  struct stat st;

  if(stat(conf.cache_dir, &st))
  {
	if(mkdir(conf.cache_dir, 0700))
	{
	  perror("Unable to create cache directory");
	  exit(EXIT_FAILURE);
	}
  }
  else if(!S_ISDIR(st.st_mode))
  {
	fprintf(stderr, "%s is not a directory", conf.cache_dir);
	exit(EXIT_FAILURE);
  }
}

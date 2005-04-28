/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 */

#ifndef TELAK_H
#define TELAK_H

#define TELAK_RCFILE "/.telak/telakrc"
#define TELAK_CACHE_DIR "/.telak/cache/"

/* X11 and Imlib2 stuff */
#include <X11/Xlib.h>
#include <Imlib2.h>

struct deskitem
{
  Imlib_Image *image;
  char *file;
  char *url;
  int w;
  int h;
  int x;
  int y;
  int refresh;
  int loaded;
  int reverse;
  struct timeval tv;
  struct deskitem *next;
};


struct config
{
  char *file;
  char *display;
  char *cache_dir;
};

void make_conf(int argc, char **argv);
void init_x();
void check_cache_dir();

#endif

/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 */

#ifndef FETCH_H
#define FETCH_H

struct remote_img
{
  char *file;
  FILE *stream;
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
int fetch(char *url, char *dest);

#endif

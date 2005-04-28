/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
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

/* cURL stuff */
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "telak.h"
#include "fetch.h"
#include "image.h"

int write_image(void *buffer, size_t size, size_t nmemb, void * data)
{
  struct remote_img * img;
  img = (struct remote_img *) data;
  
  /* open file for writing */
  if(!img->stream)
	img->stream = fopen((char *) img->file, "wb");
  
  if(!img->stream)
  {
	perror("Error creating file");
	return -1; /* failure, can't open file to write */
  }
  
  return fwrite(buffer, size, nmemb, img->stream);
}

int
fetch(char *url, char *dest)
{
  CURL *curl_handle;
  struct remote_img img;

  img.file = dest;
  img.stream = NULL;

  curl_global_init(CURL_GLOBAL_ALL);
  
  curl_handle = curl_easy_init();
  
  if(curl_handle)
  {
	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_image);
	
	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &img);
	
	/* some servers don't like requests that are made without a user-agent
	   field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, TELAK_USER_AGENT);
	
	/* get it! */
	printf("Downloading %s...\n", url);
	curl_easy_perform(curl_handle);
	
	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
  }
  
  if(img.stream)
	fclose(img.stream);
  
  curl_global_cleanup();
  
  return 0;
}



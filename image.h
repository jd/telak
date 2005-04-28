/*
 * telak - A program that display pictures in root window
 * (c) 2005 - Julien Danjou <julien@danjou.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

void draw();
int load(struct deskitem * img);
int load_img(struct deskitem * img);

#endif

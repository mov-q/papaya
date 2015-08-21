/*
 * Copyright (C) 2008 Marco Miralto <marco@teknusi.org>
 *
 * This file is part of Papaya
 *
 * Papaya is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Papaya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Papaya.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOURCES_H
#define __SOURCES_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>
#include <streamer.h>

#define MAX_DESC 256

struct input_source{
    GstElement *srcptr;
    char *name;

    GstPad *req_pad;
    char *req_pad_name;

    GstPad *q_pad;
    char *q_pad_name;

    GstPad *ghost_pad;
    char *ghost_pad_name;

    GstPad *pprobed;
    gulong ep_handler;

    char *desc;
};

struct total_sources{
    int active;
    GList *sources_l;
};

struct av_source{
    struct input_source *astruct;
    struct input_source *vstruct;
};

extern struct total_sources *globsrc;

void init_global_sources();
struct input_source* add_new_stream_source(GstElement *newsrc, const char *srclink, const char *idesc);
struct input_source* add_new_vstream_source(GstElement *newsrc, const char *srclink, const char *idesc);
struct input_source* add_new_vbox_source(GstElement *newsrc, const char *srclink, const char *idesc);
void remove_stream_source(struct input_source *isrc);
void remove_vstream_source(struct av_source *isrc);

/* low level print of source */
void _print_sources(struct input_source *s);

#endif

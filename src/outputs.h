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

#ifndef __OUTPUTS_H
#define __OUTPUTS_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>
#include "streamer.h"

struct pipe_out {
    GstElement *out_el;
    char *out_el_name;

    GstPad *tee_reqpad;
    char *tee_reqpad_name;

    GstPad *out_el_pad;
    char *out_el_pad_name; 

    char *out_desc;
};

struct global_out {
    gint active;
    GList *out_list;
};

extern struct global_out *glob_outputs;

void init_global_outputs();

struct pipe_out 
*add_new_outputs(GstElement *newout,
        const char *outlink,
        const char *outdesc);
void _print_outputs(struct pipe_out *s);
#endif

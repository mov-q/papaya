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
#ifndef __GSTV_ELEMS_H
#define __GSTV_ELEMS_H
#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <glib.h>
#include "streamglobals.h"
#include "confreader.h"
#include "pconfig.h"

GstElement* get_v4l2_source (void);
GstElement* get_videobox_source (void);
GstElement* get_videotest_source (void);
GstElement* get_video_scale (void);
GstElement* get_video_scale_capsfilter (void);
GstElement* get_h264_elem (void);

#endif

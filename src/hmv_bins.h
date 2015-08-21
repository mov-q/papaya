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
#ifndef __HMV_BINS_H
#define __HMV_BINS_H
#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <glib.h>
#include "streamglobals.h"
#include "confreader.h"
#include "pconfig.h"
#include "gsta_elems.h"
#include "gstv_elems.h"
#include "gstmisc_elems.h"

GstElement* get_videobox_source_bin (void);
GstElement* get_theora_bin (void);
GstElement* get_shout_video_out (void);
GstElement* get_rtpv_bin (void);
GstElement* get_rtp_video_out (void);
#endif

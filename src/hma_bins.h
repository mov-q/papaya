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
#ifndef __HMA_BINS_H
#define __HMA_BINS_H
#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <glib.h>
#include "streamglobals.h"
#include "confreader.h"
#include "pconfig.h"
#include "gsta_elems.h"
#include "gstmisc_elems.h"
#include "raw_at.h"
#include "raw_vt.h"
#include "hmmisc_bins_cb.h"

GstElement* get_alsa_source_bin();
GstElement* get_alsa_out();
GstElement* get_vorbis_enc();
GstElement* get_source_bin(const char *fargs);
GstElement* get_net_source_bin(const char *url);
GstElement* get_rtp_audio_out();
#endif

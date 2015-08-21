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

#ifndef __STREAMER_H
#define __STREAMER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <glib.h>
#include <gst/gst.h>
#include "streamglobals.h"
#include "hma_bins.h"
#include "hmv_bins.h"
#include "hmx_bins.h"
#include "hmmisc_bins.h"
#include "playlist.h"
#include "clioptions.h"
#include "pconfig.h"
#include "confreader.h"
#include "sources.h"
#include "outputs.h"
#include "papaya_globals.h"
#include "testsources.h"
#include "streamer_cb.h"

/* the sourcebin */
GstElement *srcbin;

/* tee elments to fork outputs for audio and video */
GstElement *atee;
GstElement *vtee;

GstElement *silence;

/* main loop */
GMainLoop *loop;


void start_stream();
void skip_stream();
void stop_stream();
void setup_streamer(int narg, char *sarg[]);
void pass_off();
void pass_on();

void _cmd_remove_source(int srcpos);
void _cmd_remove_output(int srcpos);
void _cmd_mute(int srcpos);
void _cmd_pass_on();
void _cmd_unmute(int srcpos);
int _cmd_volup(int srcpos);
int _cmd_voldown(int srcpos);
void _cmd_add_pl(gdouble vol_level);
void _cmd_add_vpl();
void _cmd_add_box();
void _cmd_add_net(const char *);
void _cmd_add_audio_live(const char *);
void _cmd_add_audio_file(const struct typed_command *lcom);

#endif

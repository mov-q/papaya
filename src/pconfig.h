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

#ifndef __PCONFIG_H
#define __PCONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <clioptions.h>
#include <papaya_globals.h>

static xmlDocPtr xml_doc;
static xmlXPathContextPtr xpath_ctx;

void papaya_config_parse (void);

enum serv_mode conf_get_server_type (void);

xmlChar* conf_get_ice_server (void);
int conf_get_ice_port (void);
int conf_get_ice_format (void);
xmlChar* conf_get_ice_username (void);
xmlChar* conf_get_ice_password (void);
xmlChar* conf_get_ice_mountpoint();
xmlChar* conf_get_ice_streamname (void);
xmlChar* conf_get_ice_description (void);
xmlChar* conf_get_ice_genre (void);

xmlChar* conf_get_rtp_server (void);
int conf_get_rtp_port (void);
int conf_get_rtp_video_format (void);
int conf_get_rtp_audio_format (void);

int conf_get_mp3_encoder_bitrate (void);
int conf_get_mp3_encoder_quality (void);
int conf_get_mp3_encoder_samplerate (void);

int conf_get_vorbis_encoder_bitrate (void);
enum encoder_type conf_get_vorbis_encoder_type (void);
float conf_get_vorbis_encoder_quality (void);
int conf_get_vorbis_encoder_samplerate (void);


int conf_get_audiosettings_channels (void);
int conf_get_audiosettings_samplerate (void);

xmlChar* conf_get_live_video_device (void);

xmlChar* conf_get_videosettings_framerate (void);
int conf_get_videosettings_width (void);
int conf_get_videosettings_height (void);

int conf_get_x_grab_startx (void);
int conf_get_x_grab_starty (void);
int conf_get_x_grab_endx (void);
int conf_get_x_grab_endy (void);

enum encoder_type conf_get_theora_encoder_type (void);
int conf_get_theora_encoder_bitrate (void);
int conf_get_theora_encoder_quality (void);

enum encoder_type conf_get_h264_encoder_type (void);
int conf_get_h264_encoder_bitrate (void);
int conf_get_h264_encoder_quality (void);

xmlChar* conf_get_live_audio_device (void);
gint64 conf_get_live_audio_bufutime (void);
gint64 conf_get_live_audio_lattime (void);

xmlChar* conf_get_live_pulse_device (void);
gint64 conf_get_live_pulse_buffer_time (void);
gint64 conf_get_live_pulse_latency_time (void);
#endif

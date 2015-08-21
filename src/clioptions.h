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

#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <playlist.h>

enum source_type
{
    AUDIO_PLAYLIST = 0,
    AUDIO_LIVE = 1,
    VIDEO_PLAYLIST = 2,
    VIDEO_LIVE = 3,
    XORG_LIVE = 4,
    DVB_LIVE = 5,
    DV1394_LIVE = 6
};

extern gboolean audiomon;
extern gboolean videomon;

/* playlist audio and video flags */
extern gboolean paudio;
extern gboolean pvideo;

/* live audio and video flags */
extern gboolean laudio;
extern gboolean lvideo;
extern gboolean ldvb;
extern gboolean ldv1394;
extern gboolean deskvideo;

/* playlist file */
extern char *plfile;
extern char *configfile;

extern gboolean SHUFFLE_MODE;
extern gboolean WITH_PULSE;
extern gboolean DAEMON_MODE;
/* stream type flag */
extern enum source_type STREAM_TYPE;

void stream_options();

#endif

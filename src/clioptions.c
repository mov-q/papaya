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

#include <clioptions.h>

/* Defaults for command-line options. */
gboolean audiomon = FALSE;
gboolean videomon = FALSE;
gboolean paudio = FALSE;
gboolean pvideo = FALSE;
gboolean laudio = FALSE;
gboolean lvideo = FALSE;
gboolean ldvb = FALSE;
gboolean ldv1394 = FALSE;
gboolean deskvideo = FALSE;
char *plfile = NULL;
char *configfile = NULL;
gboolean SHUFFLE_MODE = FALSE;
gboolean WITH_PULSE = FALSE;
enum source_type STREAM_TYPE;
gboolean DEAMON_MODE = FALSE;


void stream_options(){

    /* 
     * playlist options, this is valid 
     * for both audio and video playlist 
     */
    if(paudio && pvideo){
        g_print("Select only one between --pl-audio and --pl-video\n");
        exit (-1);
    }
    else if(paudio && plfile){
        g_print("Loading audio playlist\n");
        STREAM_TYPE = AUDIO_PLAYLIST;
        init_playlist(plfile);
    }
    else if(pvideo && plfile){
        g_print("Loading video playlist\n");
        STREAM_TYPE = VIDEO_PLAYLIST;
        init_playlist(plfile);
    }

    else if(laudio && lvideo){
        g_print("Select only one between --live-audio and --live-video\n");
        exit (-1);
    }

    else if(laudio){
        g_print("Live audio mode\n");
        STREAM_TYPE = AUDIO_LIVE;
    }

    else if(lvideo){
        g_print("Live video mode\n");
        STREAM_TYPE = VIDEO_LIVE;
    }
    else if(ldvb){
        g_print("Live dvb mode\n");
        STREAM_TYPE = DVB_LIVE;
    }
    else if(ldv1394){
        g_print("Live dv1394 mode\n");
        STREAM_TYPE = DV1394_LIVE;
    }
    else if(deskvideo){
        g_print("Desktop video transmission mode\n");
        STREAM_TYPE = XORG_LIVE;
    }
    else {
        g_print("Not valid options\n");
        exit (-1);
    }

}

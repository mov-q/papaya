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

#include <glib.h>
#include <gst/gst.h>
#include <streamer.h>
#include <playlist.h>
#include <gui.h>
#include <clioptions.h>
#include <pconfig.h>
#include <shell.h>
#include <signal.h>
#include <unistd.h>
#include "psignals.h"
#include "confreader.h"

static GOptionEntry optab[] = {
    {"pl-audio", 0, 0, G_OPTION_ARG_NONE, &paudio,
        "Load audio files from the playlist", NULL},
    {"pl-video", 0, 0, G_OPTION_ARG_NONE, &pvideo,
        "Load video files from the playlist", NULL},
    {"dvb", 0, 0, G_OPTION_ARG_NONE, &ldvb,
        "Capture from a DVB-(T|S) source", NULL},
    {"dv1394", 0, 0, G_OPTION_ARG_NONE, &ldv1394,
        "Capture from a ieee 1394 (FireWire) source", NULL},
    {"live-audio", 0, 0, G_OPTION_ARG_NONE, &laudio,
        "Use live audio input", NULL},
    {"live-video", 0, 0, G_OPTION_ARG_NONE, &lvideo,
        "Use live video input", NULL},
    {"live-x", 0, 0, G_OPTION_ARG_NONE, &deskvideo,
        "Use the desktop as a video source", NULL},
    {"playlist", 'p', 0, G_OPTION_ARG_STRING, &plfile,
        "Source playlist", "FILE"},
    {"audiomon", 0, 0, G_OPTION_ARG_NONE, &audiomon,
        "?", NULL},
    {"videomon", 0, 0, G_OPTION_ARG_NONE, &videomon,
        "?", NULL},
    {"shuffle", 0, 0, G_OPTION_ARG_NONE, &SHUFFLE_MODE,
        "Enable shuffle mode", NULL},
    {"config", 'c', 0, G_OPTION_ARG_STRING, &configfile,
        "Configuration file", "FILE"},
    {"daemon", 'd', 0, G_OPTION_ARG_NONE, &DAEMON_MODE,
        "Start in Daemon mode", NULL},
    {"with-pulse", 0, 'P', G_OPTION_ARG_NONE, &WITH_PULSE,
        "Enable pulseaudio", NULL},
    {NULL}
};

gboolean DAEMON_MODE=FALSE;

gint 
main (gint   argc, gchar *argv[]){
    GError *err = NULL;
    GOptionContext *ctx;

    /* we have a strange problem with SIGINT
     * and remote secure shells 
     */ 	

    g_thread_init(NULL);

    ctx = g_option_context_new("[MAIN OPTIONS]");
    g_option_context_add_main_entries(ctx, optab, NULL);
    g_option_context_add_group(ctx, gst_init_get_option_group());
    if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
        g_print("Error initializing: %s\n", GST_STR_NULL(err->message));
        exit(1);
    }
    g_option_context_free(ctx);

    config_read();
    papaya_config_parse();

    //gtk_init(&argc,&argv);

    stream_options();
    /* main_loop */
    loop = g_main_loop_new (NULL, FALSE);

    /* let's setup the streaming chain!!*/
    setup_streamer(argc,argv);

#ifdef HAVE_READLINE
    if(!DAEMON_MODE)
        g_thread_create(get_control_shell, NULL, NULL, NULL);
#endif
    g_main_loop_run (loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    g_print("freed!!\n");
    kill(getpid(), SIGINT);
    return 0;
};

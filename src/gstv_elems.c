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
#include "gstv_elems.h"

/* simple v4l2 source, just a stub at the moment
 * must add configuration to select a device * */
GstElement* 
get_v4l2_source()
{
    GstElement *v4l2s;
    struct p_live_video *conf;

    conf = parsed_conf->live_video_par;

    v4l2s = gst_element_factory_make("v4l2src", "m-video-source");
    g_object_set(G_OBJECT(v4l2s), "device", conf->device, NULL);

    return v4l2s;
}

GstElement* 
get_videobox_source()
{
    GstElement *vb;

    vb = gst_element_factory_make("videobox", NULL);
    g_object_set(G_OBJECT(vb), "alpha", 0.5, NULL);

    return vb;
}

GstElement* 
get_videotest_source()
{
    GstElement *vt;

    vt = gst_element_factory_make("videotestsrc", NULL);

    return vt;
}

GstElement* 
get_video_scale()
{
    GstElement *vs;

    vs = gst_element_factory_make("videoscale", NULL);
    g_object_set(G_OBJECT(vs), "method", 0, NULL);

    return vs;
}

GstElement* 
get_video_scale_capsfilter()
{
    GstElement *vscf;
    GstCaps *scaps;

    vscf = gst_element_factory_make("capsfilter", "vscf");

    scaps = gst_caps_new_simple("video/x-raw-yuv", 
            "width", G_TYPE_INT, conf_get_videosettings_width(),
            "height", G_TYPE_INT, conf_get_videosettings_height(),
            NULL);
    g_object_set (G_OBJECT(vscf), "caps", scaps, NULL);
    return vscf;
}

GstElement*
get_h264_elem (void) {
    GstElement *h264codec;
    struct p_h264_params *conf = parsed_conf->h264_par;
    
    h264codec = gst_element_factory_make("x264enc", NULL);
    if (conf->type == BITRATE){
        g_object_set(G_OBJECT(h264codec), "pass", 0, NULL);
        g_object_set(G_OBJECT(h264codec), "bitrate", conf->bitrate, NULL);
    }
    else if (conf->type == QUALITY){
        g_object_set(G_OBJECT(h264codec), "pass", 5, NULL);
        g_object_set(G_OBJECT(h264codec), "quantizer", conf->quality, NULL);
    }
    else if (conf->type == QUANTIZER){
        g_object_set(G_OBJECT(h264codec), "pass", 4, NULL);
        g_object_set(G_OBJECT(h264codec), "quantizer", conf->quality, NULL);
    }

    g_object_set(G_OBJECT(h264codec), "byte-stream", TRUE, NULL);
    
    return h264codec;
}

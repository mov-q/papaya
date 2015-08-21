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
#include "hmmisc_bins.h"

/* audio/video live source bin this needs much work and testing 
 * (and we need hardware to test it) */
GstElement* 
get_av_live_source_bin()
{
    GstElement *avlivebin;
    GstElement *vlive;
    GstElement *vraw_trans;

    GstElement *alive;
    GstElement *araw_trans;
    GstElement *avol;

    GstPad *asrcghost;
    GstPad *vsrcghost;

    avlivebin = gst_bin_new("avlivebin");

    /* v4l2 source + queue */
    vlive = get_v4l2_source();
    CHECK(gst_bin_add(GST_BIN(avlivebin), vlive));

    vraw_trans = get_raw_video_trans();
    CHECK(gst_bin_add(GST_BIN(avlivebin), vraw_trans));
    CHECK(gst_element_link(vlive, vraw_trans));

    /* alsasrc + queue */
    if(!WITH_PULSE)
        alive = get_alsa_source();
    else
        alive = get_pulse_source();
    CHECK(gst_bin_add(GST_BIN(avlivebin), alive));

    araw_trans = get_raw_audio_trans();
    CHECK(gst_bin_add(GST_BIN(avlivebin), araw_trans));
    CHECK(gst_element_link(alive, araw_trans));

    avol = gst_element_factory_make("volume", "srcvolume");
    CHECK(gst_bin_add(GST_BIN(avlivebin), avol));
    CHECK(gst_element_link(araw_trans, avol));

    /* ghostpads for the sourcebin */	
    asrcghost = gst_ghost_pad_new("asrc%d", 
            gst_element_get_static_pad(avol, "src"));
    vsrcghost = gst_ghost_pad_new("vsrc", 
            gst_element_get_static_pad(vraw_trans, "src"));
    /* add ghostpads as pads */
    gst_element_add_pad(avlivebin, asrcghost);
    gst_element_add_pad(avlivebin, vsrcghost);

    return avlivebin;

}

GstElement 
*get_rtp_output_pipe()
{
    GstElement *rtpbin;

    GstElement *aencoder;
    GstElement *audiofilter;
    GstPad *asinkpad;

    GstElement *vencoder;
    GstPad *vsinkpad;
    GstCaps *acaps;
    GstElement *rtpelem;
    gchar capstring[MAX_CAPS_STRING_SIZE];

    rtpbin = gst_bin_new(NULL);

    aencoder = get_aac_encoder();
    gst_bin_add(GST_BIN(rtpbin), aencoder);
    asinkpad = gst_element_get_static_pad(aencoder, "sink");

    /*
    g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
            "audio/mpeg, rate=96000, channels=2, mpegversion=4");

    acaps = gst_caps_from_string(capstring);
    audiofilter = gst_element_factory_make("capsfilter", "aaccaps");
    g_object_set(G_OBJECT(audiofilter), "caps", acaps, NULL);
    gst_bin_add(GST_BIN(rtpbin), audiofilter);
    gst_element_link(aencoder, audiofilter);
    */
        
    vencoder = get_rtpv_bin();
    gst_bin_add(GST_BIN(rtpbin), vencoder);
    vsinkpad = gst_element_get_static_pad(vencoder, "sink");

    printf("sono qui?\n");

    gst_element_add_pad(rtpbin, gst_ghost_pad_new("asink", asinkpad));
    gst_element_add_pad(rtpbin, gst_ghost_pad_new("vsink", vsinkpad));
        
    rtpelem = get_rtp_video_out();
    gst_bin_add(GST_BIN(rtpbin), rtpelem);

    gst_pad_link(gst_element_get_static_pad(vencoder, "src"),
            gst_element_get_static_pad(rtpelem, "vsink"));
    gst_pad_link(gst_element_get_static_pad(aencoder, "src"),
            gst_element_get_static_pad(rtpelem, "asink"));

    return rtpbin;

}

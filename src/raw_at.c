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
#include "raw_at.h"

/* raw audio transformations */
GstElement* 
get_raw_audio_trans()
{
    /* bin elements */
    GstElement *aconv;
    GstElement *asamplecf;
    GstElement *asample;
    GstElement *arate;
    GstElement *aqueue;
    GstElement *acf;
    /* this bin */
    GstElement *arawbin;
    /* needed caps */
    GstCaps *acaps;

    globconf_t *conf = parsed_conf;
    gchar capstring[MAX_CAPS_STRING_SIZE];
    gchar samplecs[MAX_CAPS_STRING_SIZE];

    arawbin = gst_bin_new(NULL);

    asample = gst_element_factory_make("audioresample", NULL);
    aconv = gst_element_factory_make("audioconvert", NULL);
    acf = gst_element_factory_make("capsfilter", "raw_audio_trans_caps");
    aqueue = gst_element_factory_make("queue2", NULL);
    arate = gst_element_factory_make("audiorate", NULL);

    if(conf->acodec == OGG){
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-raw-float, endianness=(int)1234,"\
                "rate=(int)%d, channels=(int)%d,"\
                "width=(int)32", 
                conf->audiosettings->samplerate,
                conf->audiosettings->channels);
    }
    else if (conf->acodec == MP3){
        /* depth only applies to x-raw-int */
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-raw-int, endianness=(int)1234,"\
                "rate=(int)%d, channels=(int)%d,"\
                "width=(int)16, depth=(int)16", 
                conf->audiosettings->samplerate,
                conf->audiosettings->channels);
    }
    else if (conf->acodec == AAC){
        /* depth only applies to x-raw-int */
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-raw-int, endianness=(int)1234,"\
                "rate=(int)%d, channels=(int)%d,"\
                "width=(int)16, depth=(int)16",
                conf->audiosettings->samplerate,
                conf->audiosettings->channels);
    }
#ifdef _DEBUG_
    g_printf("filtered audio caps: %s\n", capstring);
#endif
    /* set caps attribute on capsfilter element */
    acaps = gst_caps_from_string(capstring);
    g_object_set(G_OBJECT(acf), "caps", acaps, NULL);

    /* toggle aqueue for inspection */
    /* add the elements to the bin and link them */
    gst_bin_add_many(GST_BIN(arawbin), aconv, arate, acf,
            asample,  NULL);
    gst_element_link_many(aconv, asample, arate, acf, NULL);

    /* add pads to the bin */
    gst_element_add_pad(arawbin, gst_ghost_pad_new("sink",
                gst_element_get_static_pad(aconv, "sink")));
    gst_element_add_pad(arawbin, gst_ghost_pad_new("src",
                gst_element_get_static_pad(acf, "src")));

    return arawbin;
}

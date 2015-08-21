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
#include "hma_bins.h"

/* add an audio source */
GstElement* 
get_alsa_source_bin()
{
    GstElement *asrcbin;
    GstElement *alive;
    GstElement *avol;

    GstElement *raw_trans;

    GstPad *asrcghost;
    GstPad *vsrcghost;

    asrcbin = gst_bin_new("alivebin");

    if(!WITH_PULSE)
        alive = get_alsa_source();
    else
        alive = get_pulse_source();
    gst_bin_add(GST_BIN(asrcbin), alive);

    raw_trans = get_raw_audio_trans();
    gst_bin_add(GST_BIN(asrcbin), raw_trans);
    gst_element_link(alive, raw_trans);

    avol = gst_element_factory_make("volume", "srcvolume");
    gst_bin_add(GST_BIN(asrcbin), avol);
    gst_element_link(raw_trans, avol);

    asrcghost = gst_ghost_pad_new("asrc%d",
            gst_element_get_static_pad(avol, "src"));
    gst_element_add_pad(asrcbin, asrcghost);

    return asrcbin;
}

/* alsa output chain */
GstElement* 
get_alsa_out()
{
    GstElement *alsabin;
    GstElement *aqueue;
    GstElement *audioconv;
    GstElement *alsaout;
    GstElement *arate;
    GstElement *acf;

    GstPad *binpad;
    GstCaps *acaps;

    globconf_t *conf = parsed_conf;
    char capstring[MAX_CAPS_STRING_SIZE];

    alsabin = gst_bin_new(NULL);

    aqueue = gst_element_factory_make("queue2", NULL);
    gst_bin_add(GST_BIN(alsabin), aqueue);
    binpad = gst_element_get_static_pad(aqueue,"sink");
    //FIXME unref binpad

    audioconv = gst_element_factory_make("audioconvert",NULL);
    gst_bin_add(GST_BIN(alsabin), audioconv);
    gst_element_link(aqueue, audioconv);

    arate = gst_element_factory_make("audiorate", NULL);
    gst_bin_add(GST_BIN(alsabin), arate);
    gst_element_link(audioconv, arate);

    acf = gst_element_factory_make("capsfilter", NULL);
    if(conf->audio_mode == PULSE){
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-raw-float, rate=%d, channels=%d, width=32", 
                conf->audiosettings->samplerate,
                conf->audiosettings->channels);
    }
    else if (conf->audio_mode == ALSA){
        /* depth only applies to x-raw-int */
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-raw-int, rate=%d, channels=%d, width=32, depth=32", 
                conf->audiosettings->samplerate,
                conf->audiosettings->channels);
    }
#ifdef _DEBUG_
    g_printf("filtered audio caps: %s\n", capstring);
#endif
    acaps = gst_caps_from_string(capstring);
    g_object_set(G_OBJECT(acf), "caps", acaps, NULL);
    gst_bin_add(GST_BIN(alsabin), acf);
    gst_element_link(arate, acf);

    if(!WITH_PULSE)
        alsaout = gst_element_factory_make("alsasink", NULL);
    else
        alsaout = get_pulse_sink();
    gst_bin_add(GST_BIN(alsabin),alsaout);
    gst_element_link(acf, alsaout);
    
    gst_element_add_pad(alsabin,gst_ghost_pad_new("sink",binpad));
    gst_object_unref(binpad);

    return alsabin;
}

GstElement* 
get_vorbis_enc()
{

    GstElement *abin;
    GstElement *aconv;
    GstElement *conv;
    GstElement *qstream;
    GstElement *prev;
    GstElement *next;
    GstElement *arate;

    GstPad *sinkpad;
    GstPad *srcpad;

    /* create audio output */
    abin = gst_bin_new ("outvorbisbin");

    qstream = gst_element_factory_make("queue2", "qstream");
    gst_bin_add(GST_BIN(abin), qstream);	
    sinkpad = gst_element_get_static_pad(qstream,"sink");
    gst_element_add_pad (abin,gst_ghost_pad_new ("sink", sinkpad));	
    prev = qstream;

    aconv = gst_element_factory_make("audioconvert","aconvoutchain");
    next = aconv;
    gst_bin_add(GST_BIN(abin),aconv);
    gst_element_link(prev,next);
    prev = next;

    arate = gst_element_factory_make("audiorate", NULL);
    next = arate;
    gst_bin_add(GST_BIN(abin), arate);
    gst_element_link(prev, next);
    prev = next;

    g_print("aconv = %p , prev = %p\n",aconv,prev);

    next = conv = get_ogg_encoder();
    gst_bin_add(GST_BIN(abin),conv);
    gst_element_link(prev,next);

    srcpad = gst_element_get_static_pad(conv,"src");
    gst_element_add_pad (abin,gst_ghost_pad_new ("src", srcpad));

    return abin;
}

GstElement* 
get_source_bin(const char *fargs)
{
    GstElement *tempbin;
    GstElement *fsrc;
    GstElement *fdec;
    GstElement *avol;

    GstElement *vraw_trans;
    GstElement *araw_trans;

    GstPad *asrcghost;
    GstPad *vsrcghost;

    /* pass data to callback */
    struct srcbin_pad_struct *sbps;

    sbps = (struct srcbin_pad_struct *) 
        malloc(sizeof(struct srcbin_pad_struct));

    /* srcbin and its ghost pad */
    tempbin = gst_bin_new(NULL);
    sbps->mbin = tempbin;

    /* the real filesrc */
    switch (STREAM_TYPE){
        case VIDEO_PLAYLIST:
        case AUDIO_PLAYLIST:
            fsrc = get_file_source(fargs);
            break;
        case DVB_LIVE:
            CHECK(fsrc = get_dvb_source());
            break;
        case DV1394_LIVE:
            CHECK(fsrc = get_dv1394_source());
            break;
    }
    
    sbps->srcelem = fsrc;
    gst_bin_add(GST_BIN(tempbin), fsrc);

    /* the standard decodebin */
    fdec = gst_element_factory_make ("decodebin2", "fdecode");
    sbps->decelem = fdec;
    g_signal_connect (fdec, "new-decoded-pad", 
            G_CALLBACK (cb_new_decoded_pad), 
            sbps);
    /* the following are signals for decodebin2 */
    g_signal_connect (fdec, "removed-decoded-pad", 
            G_CALLBACK (cb_removed_decoded_pad), 
            NULL);

    gst_bin_add(GST_BIN(tempbin),fdec);
    gst_element_link (fsrc, fdec);

    araw_trans = get_raw_audio_trans();
    gst_bin_add(GST_BIN(tempbin), araw_trans);
    sbps->asinkel = araw_trans;

    avol = gst_element_factory_make("volume", "srcvolume");
    gst_bin_add(GST_BIN(tempbin), avol);
    sbps->srcvol;
    gst_element_link(araw_trans, avol);

    asrcghost = gst_ghost_pad_new("asrc%d",
            gst_element_get_static_pad(avol, "src"));

    /* add pads to the bin */
    gst_element_add_pad(tempbin, asrcghost);

    /* the following is used in case of video playlist */
    if(STREAM_TYPE == VIDEO_PLAYLIST ||
        STREAM_TYPE == DVB_LIVE ||
        STREAM_TYPE == DV1394_LIVE){
        vraw_trans = get_raw_video_trans();
        gst_bin_add(GST_BIN(tempbin), vraw_trans);
        sbps->vsinkel = vraw_trans;
        vsrcghost = gst_ghost_pad_new("vsrc",
                gst_element_get_static_pad(vraw_trans, "src"));
        gst_element_add_pad(tempbin, vsrcghost);
    }

    return tempbin;
}

GstElement* 
get_net_source_bin(const char *url)
{
    GstElement *tempbin;	
    GstElement *fsrc;
    GstElement *fdec;
    GstElement *avol;

    GstElement *araw_trans;
    GstElement *vraw_trans;

    GstPad *asrcghost;
    GstPad *vsrcghost;

    GstCaps *tcaps;

    struct srcbin_pad_struct *sbps;

    sbps = (struct srcbin_pad_struct *) 
        malloc(sizeof(struct srcbin_pad_struct));

    /* srcbin and its ghost pad */
    tempbin = gst_bin_new(NULL);
    sbps->mbin = tempbin;

    /* the real filesrc */
    fsrc = get_soup_source(url);
    gst_bin_add(GST_BIN(tempbin), fsrc);

    /* the standard decodebin */
    fdec = gst_element_factory_make ("decodebin", NULL);
    sbps->decelem = fdec;
    g_signal_connect (fdec, 
            "new-decoded-pad", 
            G_CALLBACK (cb_new_decoded_pad), 
            sbps);
    g_signal_connect (fdec, 
            "removed-decoded-pad", 
            G_CALLBACK (cb_removed_decoded_pad), 
            NULL);

    gst_bin_add(GST_BIN(tempbin),fdec);
    gst_element_link (fsrc, fdec);

    araw_trans = get_raw_audio_trans();
    gst_bin_add(GST_BIN(tempbin), araw_trans);
    sbps->asinkel = araw_trans;

    avol = gst_element_factory_make("volume", "srcvolume");
    gst_bin_add(GST_BIN(tempbin), avol);
    gst_element_link(araw_trans, avol);
    sbps->srcvol = avol;

    vraw_trans = get_raw_video_trans();
    gst_bin_add(GST_BIN(tempbin), vraw_trans);
    sbps->vsinkel = vraw_trans;

    asrcghost = gst_ghost_pad_new("asrc%d", 
            gst_element_get_static_pad(avol, "src"));
    vsrcghost = gst_ghost_pad_new("vsrc",
            gst_element_get_static_pad(vraw_trans, "src"));
    /* add pads to the bin */
    gst_element_add_pad(tempbin,asrcghost);
    gst_element_add_pad(tempbin,vsrcghost);

    return tempbin;
}

GstElement* 
get_rtp_audio_out()
{
    GstElement *rtpvbin;
    GstElement *rtpvsendq;
    GstElement *rtpvelem;

    GstPad *sinkpad0;

    rtpvbin = gst_bin_new("rtpaudiopipe");

    rtpvsendq = gst_element_factory_make("queue2", NULL);
    gst_bin_add(GST_BIN(rtpvbin), rtpvsendq);

    rtpvelem = gst_element_factory_make("udpsink", NULL);
    gst_bin_add(GST_BIN(rtpvbin), rtpvelem);
    g_object_set(G_OBJECT(rtpvelem), "host", "10.0.98.5", NULL);
    g_object_set(G_OBJECT(rtpvelem), "port", 9900, NULL);

    sinkpad0 = gst_element_get_request_pad(rtpvsendq, "sink");

    gst_element_add_pad(rtpvbin, gst_ghost_pad_new("sink",
                gst_element_get_static_pad(rtpvsendq,"sink")));


    gst_element_link(rtpvsendq, rtpvelem);

    return rtpvbin;
}

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
#include "gsta_elems.h"

static void 
net_alert_cb(GstElement *obj, 
        gint arg, 
        gpointer data);
/* remote stream source */
GstElement* 
get_pulse_source()
{
    GstElement *pulsein;

    pulsein = gst_element_factory_make("pulsesrc", "m-audio-source");
    g_object_set(G_OBJECT(pulsein), "device", conf_get_live_pulse_device(), NULL);
    g_object_set(G_OBJECT(pulsein), "do-timestamp", TRUE, NULL);
    g_object_set(G_OBJECT(pulsein), "buffer-time",
                 conf_get_live_pulse_buffer_time(), NULL);
    g_object_set(G_OBJECT(pulsein), "latency-time",
                 conf_get_live_pulse_latency_time(), NULL);

    return pulsein;
}

GstElement* 
get_soup_source(const char *url)
{
    GstElement *soupsrc;

    soupsrc = gst_element_factory_make("souphttpsrc", "netsrc");
    g_object_set(G_OBJECT(soupsrc), "location", url, NULL);
    g_object_set(G_OBJECT(soupsrc), "iradio-mode", TRUE, NULL);
    g_object_set(G_OBJECT(soupsrc), "do-timestamp", TRUE, NULL);
    g_object_set(G_OBJECT(soupsrc), "timeout", 10, NULL);
    return soupsrc;
}

GstElement* 
get_pulse_sink()
{
    GstElement *pulseout;

    pulseout = gst_element_factory_make("pulsesink", NULL);

    return pulseout;
}

GstElement* 
get_alsa_source()
{
    GstElement *alsainput;
    struct p_alsa_params *conf =
        parsed_conf->live_audio_par->alsa_par;

    alsainput = gst_element_factory_make("alsasrc", "m-audio-source");
    g_object_set(G_OBJECT(alsainput), "device",
            conf->device, NULL);
    g_object_set(G_OBJECT(alsainput), "buffer-time", 
            conf->buf_time, NULL);
    g_object_set(G_OBJECT(alsainput), "latency-time",
            conf->lat_time, NULL);
    g_object_set(G_OBJECT(alsainput), "do-timestamp",
            TRUE, NULL);

    return alsainput;
}

/* the adder bin mix several signals, it accepts raw audio
 * formats and it is fundamental that decoded sources use
 * an audioconvert element before linking to the adder channel
 */
GstElement* 
get_adder_bin()
{
    GstElement *addbin;
    GstElement *tq;

    addbin = gst_bin_new("addbin");
    adder = gst_element_factory_make("adder", NULL);
    gst_bin_add(GST_BIN(addbin), adder);	

    /* we start with two channels, one for the playlist,
     * another for the silence signal
     */ 
    gst_element_add_pad(addbin,
            gst_ghost_pad_new("src", 
                gst_element_get_static_pad(adder, "src")));

    return addbin;

}

GstElement* 
get_shout_out()
{
    GstElement *tempshout;
    struct p_ice_config *conf = parsed_conf->ice_server;

    tempshout = gst_element_factory_make ("shout2send", NULL);

    /* icecast configuration */
    g_object_set(G_OBJECT(tempshout),"ip", conf->hostname, NULL);
    g_object_set(G_OBJECT(tempshout),"port", conf->port, NULL);
    g_object_set(G_OBJECT(tempshout),"username", conf->username, NULL);
    g_object_set(G_OBJECT(tempshout),"password", conf->password, NULL);
    g_object_set(G_OBJECT(tempshout),"mount", conf->mountpoint, NULL);
    g_object_set(G_OBJECT(tempshout), "streamname", conf->stream_name, NULL);
    g_object_set(G_OBJECT(tempshout), "description", conf->description, NULL);
    g_object_set(G_OBJECT(tempshout), "genre", conf->genre, NULL);
    /* the pipeline seems not to support qos events */
    g_object_set(G_OBJECT(tempshout), "qos", TRUE, NULL);
    //g_object_set(G_OBJECT(tempshout), "preroll-queue-len", 50, NULL);
    //g_object_set(G_OBJECT(tempshout), "max-lateness", (gint64) 300000000, NULL);
    /* nonblocking patch */
    //g_object_set(G_OBJECT(tempshout), "nonblock", TRUE, NULL);

    /* 
     * alert in case of connection problem
     * this is bugged, cause in case of drop of connectivity, 
     * the shout_send function will block for an undefined 
     * amount of time and the callback function will only then
     * be invoked
     */
    g_signal_connect(G_OBJECT(tempshout),"connection-problem", G_CALLBACK(net_alert_cb), NULL);

    return tempshout;
};

static void 
net_alert_cb(GstElement *obj, gint arg, gpointer data)
{
    g_print("**NETALERT*** net problems\n");
}


GstElement* 
get_ogg_mux()
{
    GstElement *oggmux;

    oggmux = gst_element_factory_make("oggmux","mux");

    return oggmux;
}

GstElement* 
get_ogg_encoder()
{
    GstElement *oggencoder;
    struct p_vorbis_params *conf = parsed_conf->vorbis_par;

    oggencoder = gst_element_factory_make("vorbisenc",NULL);

    if(conf->type == QUALITY){
        g_object_set(G_OBJECT(oggencoder), 
                "bitrate", 
                -1, 
                NULL);
        g_object_set(G_OBJECT(oggencoder), 
                "quality", 
                conf->quality,
                NULL);

    }
    else if(conf->type == BITRATE){
        g_object_set(G_OBJECT(oggencoder),
                "managed",
                TRUE, NULL);
        g_object_set(G_OBJECT(oggencoder), 
                "bitrate",
                conf->bitrate, NULL);

    }
    g_printf("quality: %f\n", conf->quality);

    return oggencoder;
}

GstElement* 
get_mp3_encoder()
{
    GstElement *mp3converter;
    struct p_mp3_params *conf = parsed_conf->mp3_par;

    mp3converter = gst_element_factory_make("lame", NULL);
    g_object_set(G_OBJECT(mp3converter), "bitrate", conf->bitrate, NULL);
    g_object_set(G_OBJECT(mp3converter), "quality", conf->quality, NULL);


    return mp3converter;
}

GstElement* 
get_aac_encoder()
{
    GstElement *aacenc;

    aacenc = gst_element_factory_make("faac", NULL);

    return aacenc;
}


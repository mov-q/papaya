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
#include "hmv_bins.h"

GstElement* 
get_videobox_source_bin (void)
{
    GstElement *v4l2;
    GstElement *cspace;
    GstElement *boxbin;
    GstElement *vrate;
    GstElement *vb;
    GstElement *vtrans;

    GstPad *gpad;
    GstPad *srcpad;

    boxbin = gst_bin_new(NULL);

    v4l2 = get_v4l2_source();
    gst_bin_add(GST_BIN(boxbin), v4l2);

    cspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(GST_BIN(boxbin), cspace);
    gst_element_link(v4l2, cspace);


    vb = get_videobox_source();
    gst_bin_add(GST_BIN(boxbin), vb);
    gst_element_link(cspace, vb);

    srcpad = gst_element_get_static_pad(vb, "src");
    gpad = gst_ghost_pad_new("vsrc", srcpad);
    gst_element_add_pad(boxbin, gpad);

    gst_object_unref(GST_OBJECT(srcpad));

    return boxbin;
}

GstElement* 
get_theora_bin (void)
{
    GstElement *tbin;
    GstElement *vtee;
    GstElement *vqueue;
    GstElement *convspace;
    GstElement *tenc;
    GstElement *tover;
    GstElement *preconvspace;
    GstElement *deint;

    GstPad *tbinsink;
    GstPad *tbinsrc;
    enum encoder_type enct = conf_get_theora_encoder_type();

    tbin = gst_bin_new("theorapipe");
    
    
    vqueue = gst_element_factory_make("queue2", "theoraenc_queue");
    gst_bin_add(GST_BIN(tbin), vqueue);
    tbinsink = gst_element_get_static_pad(vqueue, "sink");

    vtee = gst_element_factory_make("tee", NULL);
    gst_bin_add(GST_BIN(tbin), vtee);
    gst_element_link(vqueue, vtee);

    preconvspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(GST_BIN(tbin), preconvspace);
    gst_element_link(vtee, preconvspace);

    deint = gst_element_factory_make("deinterlace", NULL);
    gst_bin_add(GST_BIN(tbin), deint);
    g_object_set(G_OBJECT(deint), "fields", 1, NULL);
    g_object_set(G_OBJECT(deint), "mode", 1, NULL);
    g_object_set(G_OBJECT(deint), "method", 4, NULL);
    gst_element_link(preconvspace, deint);
    
    convspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(GST_BIN(tbin), convspace);
    gst_element_link(deint, convspace);
    /*
    tover = gst_element_factory_make("textoverlay", NULL);
    g_object_set(G_OBJECT(tover), "text", "Papaya broadcaster", NULL);
    g_object_set(G_OBJECT(tover), "halignment", 0, NULL);
    g_object_set(G_OBJECT(tover), "shaded-background", TRUE, NULL);
    gst_bin_add(GST_BIN(tbin), tover);
    gst_element_link(convspace,tover);
    */

    tenc = gst_element_factory_make("theoraenc", NULL);
    tbinsrc = gst_element_get_static_pad(tenc, "src");
    if (enct == BITRATE){
        g_object_set(G_OBJECT(tenc), "bitrate", conf_get_theora_encoder_bitrate(), NULL);
    }
    else if (enct == QUALITY){
        g_object_set(G_OBJECT(tenc), "bitrate", 0, NULL);
        g_object_set(G_OBJECT(tenc), "quality", conf_get_theora_encoder_quality(), NULL);
    }

    g_object_set(G_OBJECT(tenc), "speed-level", 2, NULL);
    
    gst_bin_add(GST_BIN(tbin), tenc);
    gst_element_link(convspace, tenc);

    gst_element_add_pad(tbin, gst_ghost_pad_new("sink", tbinsink));
    gst_element_add_pad(tbin, gst_ghost_pad_new("src", tbinsrc));

    gst_object_unref(GST_OBJECT(tbinsink));
    gst_object_unref(GST_OBJECT(tbinsrc));

    return tbin;
}

GstElement* 
get_shout_video_out (void)
{
    GstElement *svbin;
    GstElement *vshout;
    GstElement *omux;
    GstElement *asendq;
    GstElement *vsendq;

    GstPad *sinkpad0;
    GstPad *sinkpad1;

    svbin = gst_bin_new("svpipe");

    omux = gst_element_factory_make("oggmux", "omux");
    gst_bin_add(GST_BIN(svbin), omux);

    asendq = gst_element_factory_make("queue2", "a_shout_queue");
    gst_bin_add(GST_BIN(svbin), asendq);

    vsendq = gst_element_factory_make("queue2", "v_shout_queue");
    gst_bin_add(GST_BIN(svbin), vsendq);

    vshout = get_shout_out();
    gst_bin_add(GST_BIN(svbin), vshout);

    sinkpad0 = gst_element_get_request_pad(omux, "sink_%d");
    gst_element_add_pad(svbin, gst_ghost_pad_new("sink_0",
                gst_element_get_static_pad(asendq,"sink")));

    sinkpad1 = gst_element_get_request_pad(omux, "sink_%d");
    gst_element_add_pad(svbin, gst_ghost_pad_new("sink_1",
                gst_element_get_static_pad(vsendq,"sink")));


    gst_pad_link(gst_element_get_static_pad(asendq, "src"), sinkpad0);
    gst_pad_link(gst_element_get_static_pad(vsendq, "src"), sinkpad1);

    gst_element_link(omux, vshout);

    return svbin;
}

GstElement*
get_videotest_bin (void)
{
    GstElement *vtestbin;
    GstElement *vtest;
    GstElement *vraw;

    vtestbin = gst_bin_new(NULL);

    vtest = get_videotest_source();
    gst_bin_add(GST_BIN(vtestbin), vtest);
    
    vraw = get_raw_video_trans();
    gst_bin_add(GST_BIN(vtestbin), vraw);
    gst_element_link(vtest, vraw);
    
    gst_element_add_pad(vtestbin, gst_ghost_pad_new("tsrc",
        gst_element_get_static_pad(vraw, "src")));

    return vtestbin;
}

GstElement* 
get_rtpv_bin (void)
{
    GstElement *rtpvencbin;
    GstElement *vtee;
    GstElement *vqueue;
    GstElement *convspace;
    GstElement *tover;
    GstElement *venc;

    GstPad *rtpvencbinsink;
    GstPad *rtpvencbinsrc;

    struct p_rtp_config *conf = parsed_conf->rtp_server;

    rtpvencbin = gst_bin_new(NULL);

    vqueue = gst_element_factory_make("queue2", "rtpenc_queue" );
    gst_bin_add(GST_BIN(rtpvencbin), vqueue);
    rtpvencbinsink = gst_element_get_static_pad(vqueue, "sink");
    
    vtee = gst_element_factory_make("tee", NULL);
    gst_bin_add(GST_BIN(rtpvencbin), vtee);
    gst_element_link(vqueue,vtee);

    convspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(GST_BIN(rtpvencbin), convspace);
    gst_element_link(vtee, convspace);

    tover = gst_element_factory_make("textoverlay", NULL);
    g_object_set(G_OBJECT(tover), "text", "Papaya broadcaster", NULL);
    g_object_set(G_OBJECT(tover), "halignment", 0, NULL);
    g_object_set(G_OBJECT(tover), "shaded-background", TRUE, NULL);
    gst_bin_add(GST_BIN(rtpvencbin), tover);
    gst_element_link(convspace,tover);
    
    switch (conf->vformat) {
        case H264:
            venc = get_h264_elem();
            break;
        case MPEG4:
            venc = NULL;
            break;
    };
    
    gst_bin_add(GST_BIN(rtpvencbin), venc);
    gst_element_link(tover, venc);
    rtpvencbinsrc = gst_element_get_static_pad(venc, "src");

    gst_element_add_pad(rtpvencbin, gst_ghost_pad_new("src", rtpvencbinsrc));
    gst_element_add_pad(rtpvencbin, gst_ghost_pad_new("sink", rtpvencbinsink));

    gst_object_unref(GST_OBJECT(rtpvencbinsink));
    gst_object_unref(GST_OBJECT(rtpvencbinsrc));

    return rtpvencbin;
}

GstElement* 
get_rtp_video_out (void)
{
    GstElement *rtpvbin;
    GstElement *rtpvsendq;
    GstElement *rtpasendq;
    GstElement *h264mux;
    GstElement *rtpmux;
    GstElement *rtpvelem;
    GstElement *h264pay, *mp4apay;
    GstElement *fake;
    GstElement *mprtp;

    GstPad *videosink;
    GstPad *audiosink;
    GstPad *vreq, *areq;

    rtpvbin = gst_bin_new("rtpvideopipe");

    h264mux = gst_element_factory_make("mpegtsmux", NULL);
    gst_bin_add(GST_BIN(rtpvbin), h264mux);
    videosink = gst_element_get_request_pad(h264mux, "sink_%d");
    audiosink = gst_element_get_request_pad(h264mux, "sink_%d");

    /*
    h264pay = gst_element_factory_make("rtph264pay", NULL);
    gst_bin_add(GST_BIN(rtpvbin), h264pay);
    videosink = gst_element_get_static_pad(h264pay, "sink");

    mp4apay = gst_element_factory_make("rtpmp4apay", NULL);
    gst_bin_add(GST_BIN(rtpvbin), mp4apay);
    audiosink = gst_element_get_static_pad(mp4apay, "sink");
    
    rtpmux = gst_element_factory_make("rtpmux", NULL);
    gst_bin_add(GST_BIN(rtpvbin), rtpmux);
    gst_pad_link(gst_element_get_static_pad(h264pay, "src"),
        gst_element_get_request_pad(rtpmux, "sink_%d"));
    gst_pad_link(gst_element_get_static_pad(mp4apay, "src"),
        gst_element_get_request_pad(rtpmux, "sink_%d"));
    */

    
    mprtp = gst_element_factory_make("rtpmp2tpay", NULL);
    gst_bin_add(GST_BIN(rtpvbin), mprtp);
    gst_element_link(h264mux, mprtp);

    
    rtpvelem = gst_element_factory_make("udpsink", NULL);
    gst_bin_add(GST_BIN(rtpvbin), rtpvelem);
    g_object_set(G_OBJECT(rtpvelem), "host", "224.1.2.3", NULL);
    g_object_set(G_OBJECT(rtpvelem), "port", 5000, NULL);
    //gst_element_link(rtpmux, rtpvelem);
    gst_element_link(mprtp, rtpvelem);
    /*
    fake = gst_element_factory_make("filesink", NULL);
    g_object_set(G_OBJECT(fake), "location", "prova.mp4", NULL);
    gst_bin_add(GST_BIN(rtpvbin), fake);
    gst_element_link(mprtp, fake);
    */
    gst_element_add_pad(rtpvbin, gst_ghost_pad_new("vsink", 
                videosink));
    gst_element_add_pad(rtpvbin, gst_ghost_pad_new("asink",
                audiosink));
    
    gst_object_unref(GST_OBJECT(videosink));
    gst_object_unref(GST_OBJECT(audiosink));

    return rtpvbin;
}

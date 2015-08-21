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
#include "hmx_bins.h"

/* the source bin for live desktop streaming 
 * it includes a live alsa source together with
 * desktop streaming */
GstElement* 
get_x_source_bin()
{
    GstElement *xsrcbin;
    GstElement *xsrc;
    GstElement *vraw_trans;

    GstElement *alive;
    GstElement *araw_trans;
    GstElement *avol;

    GstPad *asrcghost;
    GstPad *vsrcghost;

    GstCaps *vcaps;


    xsrcbin = gst_bin_new("xsourcebin");

    /* grab xorg desktop and queue */
    xsrc = get_x_source();
    gst_bin_add(GST_BIN(xsrcbin), xsrc);

    vraw_trans = get_raw_video_trans();
    gst_bin_add(GST_BIN(xsrcbin), vraw_trans);
    gst_element_link(xsrc, vraw_trans);

    /* audiolive chain */
    if(!WITH_PULSE)
        alive = get_alsa_source();
    else
        alive = get_pulse_source();
    gst_bin_add(GST_BIN(xsrcbin), alive);

    araw_trans = get_raw_audio_trans();
    gst_bin_add(GST_BIN(xsrcbin), araw_trans);
    gst_element_link(alive, araw_trans);

    avol = gst_element_factory_make("volume", "srcvolume");
    gst_bin_add(GST_BIN(xsrcbin), avol);
    gst_element_link(araw_trans, avol);

    asrcghost = gst_ghost_pad_new("asrc%d", 
            gst_element_get_static_pad(avol, "src"));
    vsrcghost = gst_ghost_pad_new("vsrc", 
            gst_element_get_static_pad(vraw_trans, "src"));
    gst_element_add_pad(xsrcbin, asrcghost);
    gst_element_add_pad(xsrcbin, vsrcghost);

    return xsrcbin;

}

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
#include "raw_xt.h"

//FIXME: adjust configuration parameters
GstElement* 
get_raw_videobox_trans()
{
    /* elements in the bin */
    GstElement *vcolspace;
    GstElement *vrate;
    GstElement *vqueue;
    GstElement *vcf;
    GstElement *vscale;
    /* this bin */
    GstElement *vrawbin;
    /* needed caps */
    GstCaps *vcaps;

    vrawbin = gst_bin_new(NULL);

    gchar capstring[MAX_CAPS_STRING_SIZE];

    vcolspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    vrate = gst_element_factory_make("videorate", NULL);
    vqueue = gst_element_factory_make("queue2", NULL);
    vscale = gst_element_factory_make("videoscale", NULL);
    vcf = gst_element_factory_make("capsfilter", NULL);

    g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
            "video/x-raw-yuv,\
            framerate=10/1,\
            width=128,\
            height=96");
    vcaps = gst_caps_from_string(capstring);
    g_object_set(G_OBJECT(vcf), "caps", vcaps, NULL);

    gst_bin_add_many(GST_BIN(vrawbin), vcolspace, 
            vrate, vqueue, vscale, vcf, NULL);
    gst_element_link_many(vcolspace, vrate, vqueue, 
            vscale, vcf, NULL);


    gst_element_add_pad(vrawbin, gst_ghost_pad_new("sink", 
                gst_element_get_static_pad(vcolspace, "sink")));
    gst_element_add_pad(vrawbin, gst_ghost_pad_new("src", 
                gst_element_get_static_pad(vcf, "src")));

    return vrawbin;
}

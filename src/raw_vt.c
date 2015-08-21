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

GstElement* 
get_raw_video_trans()
{
    /* bin elements */
    GstElement *vcolspace;
    GstElement *vrate;
    GstElement *vparse;
    GstElement *vcf;
    GstElement *vscale;
    /* this bin */
    GstElement *vrawbin;
    /* needed caps */
    GstCaps *vcaps;

    struct p_video_settings *vsets = parsed_conf->videosettings;

    vrawbin = gst_bin_new(NULL);

    gchar capstring[MAX_CAPS_STRING_SIZE];

    vrate = gst_element_factory_make("videorate", NULL);
    vscale = gst_element_factory_make("videoscale", NULL);
    vcolspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    vcf = gst_element_factory_make("capsfilter", NULL);

    g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
            "video/x-raw-yuv,framerate=%s, width=%d, height=%d",
            vsets->framerate,
            vsets->width,
            vsets->height);
    vcaps = gst_caps_from_string(capstring);
#ifdef _DEBUG_
    g_printf("filtered video caps: %s\n", capstring);
#endif
    g_object_set(G_OBJECT(vcf), "caps", vcaps, NULL);
    
    /* toggle vqueue for inspection */
    gst_bin_add_many(GST_BIN(vrawbin), vrate, 
            vscale,
            vcolspace,
            vcf,
            NULL);
    gst_element_link_many(vrate, vscale, vcolspace, vcf, NULL);


    gst_element_add_pad(vrawbin, gst_ghost_pad_new("sink", 
                gst_element_get_static_pad(vrate, "sink")));
    gst_element_add_pad(vrawbin, gst_ghost_pad_new("src", 
                gst_element_get_static_pad(vcf, "src")));

    return vrawbin;
}



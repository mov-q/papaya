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
#include "gstx_elems.h"

/* 
 * a simple ximagesrc element
 * FIX: for high endx/endy the application crashes
 */
GstElement* 
get_x_source()
{
    GstElement *xs;
    struct x_grab_settings *xconf = parsed_conf->xgrabsettings;

    xs = gst_element_factory_make("ximagesrc", NULL);
    //g_object_set(G_OBJECT(xs), "use-damage", FALSE, NULL);
    g_object_set(G_OBJECT(xs), "startx", xconf->startx, NULL);
    g_object_set(G_OBJECT(xs), "starty", xconf->starty, NULL);
    g_object_set(G_OBJECT(xs), "endx", xconf->endx, NULL);
    g_object_set(G_OBJECT(xs), "endy", xconf->endy, NULL);

    return xs;
}

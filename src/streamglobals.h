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
#ifndef __STREAMGLOBALS_H
#define __STREAMGLOBALS_H

#define CHECK(x) if(!(x)) {						\
    g_print("CHECK(" #x ") FAILED at %s:%d\n", __FILE__, __LINE__);	\
    exit(3);							\
}

#define MAX_CAPS_STRING_SIZE 512

/* aux struct */
struct srcbin_pad_struct{
    GstElement *mbin;
    GstElement *asinkel;
    GstElement *vsinkel;
    GstElement *decelem;
    GstElement *srcelem;
    GstElement *srcvol;
};
/* global variables to the chain */
extern GstElement *pipeline;
/* pipeline clock */
extern GstClock *pipeclock;

/* the adderbin and the adder element */
GstElement *adderbin;
GstElement *adder;
GstElement *gvmixer;

#endif

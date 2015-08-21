/*
 * Copyright (C) 2008 Marco Miralto <marco@teknusi.org>
 * Copyright (C) 2008 Luca Tamburo  <luca@teknusi.org>
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

#ifndef __PLAYLIST_H
#define __PLAYLIST_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

struct pl_element{
    char *file;
    gboolean played;
};

struct papaya_playlist{
    struct pl_element **p_element;
    unsigned int p_current;
    int p_next;
    int p_tracks;
    int p_played;
};

struct papaya_playlist m_pl;

void print_playlist(void); /*DEBUG_LUCA only for debug*/
char* get_current_track_name();
void init_playlist(char *);
struct pl_element **read_playlist(char *);

#endif

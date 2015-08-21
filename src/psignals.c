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
#include "psignals.h"

void p_sigint(int sgn){
    g_print("\n*** Oooops!! *** Ctrl-C sequence, we're stopping here\n");
    signal(SIGINT, SIG_DFL);

    g_main_loop_quit(loop);

}

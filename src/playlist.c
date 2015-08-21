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

#include <playlist.h>

void 
print_playlist (void)
{
    /*DEBUG_LUCA only for debug*/
    int i = 0;
    g_print("\nPLAYLIST\n");
    g_print("number of elements %d,  %d elments are played\n",m_pl.p_tracks,m_pl.p_played);
    g_print("p_current %d p_next  %d",m_pl.p_current, m_pl.p_next);
    g_print("\n\nPRINT OF PLAYLIST\n");
    for (i = 0; i < m_pl.p_tracks; i++){
        g_print("%d) %s ",i,m_pl.p_element[i]->file);
        if (m_pl.p_element[i]->played == TRUE)
            printf ("[played]\n");
        else
            printf ("[not played]\n");
    }
}

    char* get_current_track_name(){
        if (m_pl.p_element == NULL)
            return NULL;
        return m_pl.p_element[m_pl.p_current]->file;
    }

void 
init_playlist (char *plopt)
{
    /* set to zero the index */
    m_pl.p_current = -1;
    m_pl.p_next = 0;
    m_pl.p_played = 0;
    m_pl.p_tracks = 0;
    m_pl.p_element = read_playlist(plopt);
};

/* read playlist file and load papaya struct playlist */
struct pl_element** 
read_playlist(char *pfile)
{
    FILE *mu3file;
    struct pl_element **tmp_list;
    char tempbuf[1024];
    int i = 0, col = 0;
    char c;

    tmp_list = (struct pl_element **)malloc(sizeof(struct pl_element *));
    mu3file = fopen(pfile,"r");
    while((c = fgetc(mu3file)) != EOF){
        /* if first character of line is # skip line */
        if (c == '#')
            while ((c = fgetc(mu3file)) != '\n');
        else{
            i++;
            tmp_list = (struct pl_element **)realloc(tmp_list,i*sizeof(struct pl_element *));
            col = 0;
            while((c != '\n') && (c != EOF)){
                tempbuf[col++] = c;
                c =  fgetc(mu3file);
            }
            tempbuf[col] = '\0';
            tmp_list[i-1]=(struct pl_element *)malloc(sizeof (struct pl_element));
            tmp_list[i-1]->file = (char *)malloc((strlen(tempbuf)+1) * sizeof(char));
            tmp_list[i-1]->played = FALSE; 
            strcpy(tmp_list[i-1]->file,tempbuf);
        }

    }

    fclose(mu3file);
    m_pl.p_tracks = i;
    return tmp_list;
}

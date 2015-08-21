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
#include "outputs.h"

struct global_out *glob_outputs = NULL;

void 
init_global_outputs()
{
    /* allocate one global structure */
    glob_outputs = (struct global_out *) malloc(sizeof(struct global_out));
    glob_outputs->out_list = NULL;	
    /* init the active sources to zero initially */
    glob_outputs->active = 0;
}

struct pipe_out 
*add_new_outputs(GstElement *newout,
        const char *outlink,
        const char *outdesc)
{
    struct pipe_out *nout;

    nout = (struct pipe_out *) malloc(sizeof(struct pipe_out));

    if(outdesc){
        nout->out_desc = (char *) malloc(sizeof(char) * 
                (strlen(outdesc) + 1));
        strcpy(nout->out_desc, outdesc);
    }
    else
        nout->out_desc = NULL;
    nout->tee_reqpad = gst_element_get_request_pad(atee, "src%d");
    nout->tee_reqpad_name = gst_element_get_name(nout->tee_reqpad);

    nout->out_el = newout;
    nout->out_el_name = gst_element_get_name(newout);
    nout->out_el_pad = gst_element_get_static_pad(nout->out_el, outlink);
    nout->out_el_pad_name = gst_element_get_name(nout->out_el_pad);

    /* link a new output */
    gst_pad_link(nout->tee_reqpad,
            gst_element_get_static_pad(newout, outlink ));
    /* increment the active outputs counter */
    glob_outputs->out_list = g_list_append(glob_outputs->out_list, nout);
    glob_outputs->active = g_list_length(glob_outputs->out_list);

    return nout;

}

void 
_print_outputs(struct pipe_out *s)
{
    GList *cl = glob_outputs->out_list;

    g_print("\n%s**** Output %s ****%s\n",
            PAPAYA_BLUEL,
            s->out_el_name,
            PAPAYA_QCOLOR);
    //g_list_position(cl, g_list_find(cl, s));
    if(s->out_desc)
        g_print("%sDescription:%s %s \n", 
                PAPAYA_REDL,
                PAPAYA_QCOLOR,
                s->out_desc);
    g_print("%sPosition ID:%s %d\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            g_list_position(cl, g_list_find(cl, s)));
    g_print("%sRequest pad on tee element:%s %s\n", 
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->tee_reqpad_name);
    g_print("%sSrcPtr:%s %p\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->out_el);
}

void 
remove_stream_output(struct pipe_out *outr)
{
    GList *cl;	


    cl = glob_outputs->out_list;
    /* put the element to GST_STATE_READY then unlink
     * it and remove ghost and request pads
     */
    gst_element_set_state(outr->out_el, GST_STATE_READY);
    gst_element_unlink(outr->out_el, atee);
    gst_element_release_request_pad(atee, outr->tee_reqpad);

    /* remove the element from the pipeline */
    gst_element_set_state(outr->out_el, GST_STATE_NULL);
    gst_bin_remove(GST_BIN(pipeline), outr->out_el);

    glob_outputs->out_list = g_list_remove(cl, outr);
    /* these must be freed */
    free(outr->tee_reqpad_name);
    free(outr->out_el_name);
    free(outr->out_desc);
    free(outr);

    /* decrease the active sources counter */
    glob_outputs->active = g_list_length(glob_outputs->out_list);	
}

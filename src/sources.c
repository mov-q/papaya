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
#include <sources.h>

struct total_sources *globsrc = NULL;

void 
init_global_sources()
{
    /* allocate one global structure */
    globsrc = (struct total_sources *) malloc(sizeof(struct total_sources));
    globsrc->sources_l = NULL;	
    /* init the active sources to zero initially */
    globsrc->active = 0;
}

/* this adds a new audio source */
struct input_source* 
add_new_stream_source(GstElement *newsrc, 
        const char *srclink,
        const char *idesc)
{
    GstElement *sq;
    struct input_source *isrc;
#ifdef _DEBUG_
    g_print("Entry point for adding\n");
#endif
    /* allocate a new input_source structure */
    isrc = (struct input_source *)malloc(sizeof(struct input_source));
    isrc->pprobed = NULL;
    if(idesc){
        isrc->desc = (char *)malloc(sizeof(char) * (strlen(idesc) + 1));
        strcpy(isrc->desc, idesc);
    }
    else
        isrc->desc = NULL;
    isrc->srcptr = newsrc;
    /* let's get the name of the new source */
    isrc->name = gst_element_get_name(newsrc);

    /* request a new pad from the adder element */
    isrc->req_pad = gst_element_get_request_pad(adder, "sink%d");
    isrc->req_pad_name = gst_element_get_name(isrc->req_pad);	
    
    /* get a new ghost pad and activate it*/
    isrc->ghost_pad = gst_ghost_pad_new(isrc->req_pad_name,
            isrc->req_pad); 
    gst_pad_set_active(isrc->ghost_pad, TRUE);
    isrc->ghost_pad_name = gst_pad_get_name(isrc->ghost_pad);

    /* add a pad to the adder */
    gst_element_add_pad(adderbin,isrc->ghost_pad); 
    /* link a new source */
    gst_pad_link(gst_element_get_static_pad(newsrc, srclink ), 
            gst_element_get_static_pad(adderbin,isrc->ghost_pad_name));
    /* increment the active source counter */
    globsrc->sources_l = g_list_append(globsrc->sources_l, isrc);
    globsrc->active = g_list_length(globsrc->sources_l);

    return isrc;
}

struct input_source* 
add_new_vstream_source(GstElement *newsrc, 
        const char *srclink,
        const char *idesc)
{
    struct input_source *isrc;

    /* allocate a new input_source structure */
    isrc = (struct input_source *)malloc(sizeof(struct input_source));
    isrc->pprobed = NULL;
    if(idesc){
        isrc->desc = (char *)malloc(sizeof(char) * (strlen(idesc) + 1));
        strcpy(isrc->desc, idesc);
    }
    else
        isrc->desc = NULL;
    isrc->srcptr = newsrc;
    /* let's get the name of the new source */
    isrc->name = gst_element_get_name(newsrc);

    /* request a new pad from the adder element */
    isrc->req_pad = gst_element_get_request_pad(gvmixer, "sink_%d");
    isrc->req_pad_name = gst_element_get_name(isrc->req_pad);	

    /* link a new source */
    gst_pad_link(gst_element_get_static_pad(newsrc, srclink ), 
            isrc->req_pad);
    /* increment the active source counter */
    globsrc->sources_l = g_list_append(globsrc->sources_l, isrc);
    globsrc->active = g_list_length(globsrc->sources_l);

    return isrc;
}

struct input_source* 
add_new_vbox_source(GstElement *newsrc, 
        const char *srclink,
        const char *idesc)
{
    struct input_source *isrc;

    /* allocate a new input_source structure */
    isrc = (struct input_source *)malloc(sizeof(struct input_source));
    isrc->pprobed = NULL;
    if(idesc){
        isrc->desc = (char *)malloc(sizeof(char) * (strlen(idesc) + 1));
        strcpy(isrc->desc, idesc);
    }
    else
        isrc->desc = NULL;
    isrc->srcptr = newsrc;
    /* let's get the name of the new source */
    isrc->name = gst_element_get_name(newsrc);

    /* request a new pad from the adder element */
    isrc->req_pad = gst_element_get_request_pad(gvmixer, "sink_%d");

    isrc->req_pad_name = gst_element_get_name(isrc->req_pad);	

    /* link a new source */
    gst_pad_link(gst_element_get_static_pad(newsrc, srclink ), 
            isrc->req_pad);
    /* increment the active source counter */
    globsrc->sources_l = g_list_append(globsrc->sources_l, isrc);
    globsrc->active = g_list_length(globsrc->sources_l);

    return isrc;
}
/* removes an audio source for the strem */
void 
remove_stream_source(struct input_source *isrc)
{
    GList *cl;	

    /* FIXME check the sbps thing  */
    if(isrc->pprobed){
        gst_pad_remove_event_probe(isrc->pprobed, isrc->ep_handler);
    }

    cl = globsrc->sources_l ;
    /* put the element to GST_STATE_READY then unlink
     * it and remove ghost and request pads
     */
    gst_element_set_locked_state(isrc->srcptr, TRUE);
    //gst_element_set_state(isrc->srcptr, GST_STATE_READY);
    gst_element_set_state(isrc->srcptr, GST_STATE_NULL);
    gst_element_unlink(isrc->srcptr, adder);
    gst_element_remove_pad(adderbin, isrc->ghost_pad);
    gst_object_unref(isrc->ghost_pad);

    /* remove the element from the pipeline */
    //gst_element_set_state(isrc->srcptr, GST_STATE_NULL);
    gst_bin_remove(GST_BIN(pipeline), isrc->srcptr);
    gst_pad_send_event (isrc->req_pad, gst_event_new_eos ());
    gst_element_release_request_pad(adder, isrc->req_pad);
    gst_object_unref(isrc->req_pad);

    globsrc->sources_l = g_list_remove(cl, isrc);
    /* these must be freed */
    free(isrc->req_pad_name);
    free(isrc->ghost_pad_name);
    free(isrc->desc);
    free(isrc);

    /* decrease the active sources counter */
    globsrc->active = g_list_length(globsrc->sources_l);	
}

void 
remove_vstream_source(struct av_source *isrc)
{
    GList *cl;
    struct input_source *raudio;
    struct input_source *rvideo;

    raudio = isrc->astruct;
    rvideo = isrc->vstruct;

    if(raudio->pprobed){
        gst_pad_remove_event_probe(raudio->pprobed, raudio->ep_handler);
    }

    if(rvideo->pprobed){
        gst_pad_remove_event_probe(rvideo->pprobed, rvideo->ep_handler);
    }

#ifdef _DEBUG_
    g_print("Remove entry point\n");
#endif
    cl = globsrc->sources_l ;
    /* put the element to GST_STATE_READY then unlink
     * it and remove ghost and request pads
     */
    gst_element_set_locked_state(rvideo->srcptr, TRUE);
    //gst_element_set_state(rvideo->srcptr, GST_STATE_READY);
    gst_element_set_state(rvideo->srcptr, GST_STATE_NULL);
    gst_pad_unlink(gst_element_get_static_pad(rvideo->srcptr, "vsrc"), 
            gst_element_get_static_pad(gvmixer, rvideo->req_pad_name));
    gst_element_release_request_pad(gvmixer, rvideo->req_pad);

    gst_pad_unlink(gst_element_get_static_pad(raudio->srcptr, "asrc%d"), 
            gst_element_get_static_pad(adderbin, raudio->req_pad_name));
    gst_element_release_request_pad(adder, raudio->req_pad);

    gst_element_remove_pad(adderbin, raudio->ghost_pad);

    /* remove the element from the pipeline */
    gst_bin_remove(GST_BIN(pipeline), rvideo->srcptr);

    globsrc->sources_l = g_list_remove(cl, raudio);
    globsrc->sources_l = g_list_remove(cl, rvideo);
    /* these must be freed */
    free(raudio->req_pad_name);
    free(rvideo->req_pad_name);
    free(raudio->ghost_pad_name);
    if(raudio->desc)
        free(raudio->desc);
    if(rvideo->desc)
        free(rvideo->desc);
    free(raudio);
    free(rvideo);
    free(isrc);

    /* decrease the active sources counter */
    globsrc->active = g_list_length(globsrc->sources_l);

}

void 
_print_sources(struct input_source *s)
{
    GList *cl = globsrc->sources_l;

    g_print("\n%s**** Source %s ****%s\n",
            PAPAYA_BLUEL,
            s->name,
            PAPAYA_QCOLOR);
    g_list_position(cl, g_list_find(cl, s));
    g_print("%sDescription:%s %s \n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->desc);
    g_print("%sPosition ID:%s %d\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            g_list_position(cl, g_list_find(cl, s)));
    g_print("%sRequest pad on adder:%s %s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->req_pad_name);
    g_print("%sGhost pad name on adderbin:%s %s\n", 
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->ghost_pad_name);
    g_print("%sSrcPtr:%s %p\n", 
            PAPAYA_REDL,
            PAPAYA_QCOLOR,
            s->srcptr);
}

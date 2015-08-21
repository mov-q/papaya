#include "streamer_cb.h"


static void net_unlink(gpointer *mdata);
static void get_next_pl(gdouble vol_level);
gboolean heos(GstPad *pad, GstEvent *te, gpointer u_data);

gboolean 
net_event(GstPad *pad, 
        GstEvent *te, 
        gpointer u_data)
{
#ifdef _DEBUG_
    g_print("Net event callback : %s\n", GST_EVENT_TYPE_NAME(te));
#endif
    switch (GST_EVENT_TYPE(te)){
        case GST_EVENT_EOS: 
            {
                g_thread_create((GThreadFunc)net_unlink,
                        u_data, TRUE, NULL);
                return FALSE; 
            }
        default: 
            {
                return TRUE;
            }
    }
}

static void 
net_unlink(gpointer *mdata)
{
    struct input_source *tr;

    tr = (struct input_source *) mdata;
    g_print("%p\n", tr->srcptr);
    remove_stream_source(tr);
}

/* this callback is linked to the EOS signal
 * and gets executed in a newly created thread.
 * it creates a new filesrc element and substitute the
 * just played one */
void 
cb_change_track2(gpointer *mdata)
{
    struct srcbin_pad_struct *cbs;
    struct input_source *tro;
    GstElement *ns;
    GstElement *svol;
    gdouble oldvol;

    /* we remove the drained srcbin */
    tro = (struct input_source *)mdata;
#ifdef _DEBUG_
    GstCaps *negcaps;
    negcaps = gst_pad_get_negotiated_caps(
            gst_element_get_static_pad(adder,tro->req_pad_name));
    g_printf("negotiated caps: %s\n",gst_caps_to_string(negcaps));
#endif
    svol = gst_bin_get_by_name(GST_BIN(tro->srcptr), "srcvolume");
    g_object_get(G_OBJECT(svol), "volume", &oldvol, NULL);
#ifdef _DEBUG_
    g_print("volume in change : %f\n", oldvol);
#endif
    remove_stream_source(tro);

    /* we add a new source bin */
    get_next_pl(oldvol);

}

static void 
get_next_pl(gdouble vol_level)
{
    GstElement *pls;
    GstElement *tprobe;
    struct input_source *pl_in;
    GstElement *nvol;

    pls = get_source_bin(NULL);
    tprobe = pls;

    nvol = gst_bin_get_by_name(GST_BIN(pls), "srcvolume");
    g_object_set(G_OBJECT(nvol), "volume", vol_level, NULL);

    gst_bin_add(GST_BIN(pipeline), pls);

    pl_in = add_new_stream_source(pls, "asrc%d", "playlist source");
    pl_in->ep_handler = gst_pad_add_event_probe(
            gst_element_get_static_pad(
                tprobe, 
                "asrc%d"),
            G_CALLBACK(cb_eos_in_filesrc), pl_in);
    gst_element_set_state(pls, GST_STATE_PLAYING);
    pl_in->pprobed = gst_element_get_static_pad(tprobe, "asrc%d");
}

gboolean cb_eos_in_filesrc(GstPad *pad, 
        GstEvent *te,
        gpointer u_data)
{
    GstElement *newtemp;
    switch GST_EVENT_TYPE(te){
        case GST_EVENT_EOS:{
#ifdef _DEBUG_
                               g_print("EOS in filesrc\n");
#endif
                               g_thread_create((GThreadFunc)cb_change_track2, 
                                       u_data, 
                                       TRUE,
                                       NULL);
                               return FALSE;
                           }
        default:{
                    /* returning true does not stop 
                     * handling of other events */
                    return TRUE;
                }
    }
}


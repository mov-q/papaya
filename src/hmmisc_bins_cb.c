#include "hmmisc_bins_cb.h"

/* this callback handle a newly created sometimes pad 
 * by the decodebin */
void 
cb_new_decoded_pad(GstElement *decodebin,
                    GstPad *newpad,
                    gboolean last,
                    gpointer data)
{
    GstCaps *caps;
    GstStructure *str;
    struct srcbin_pad_struct *binpads;

    binpads = (struct srcbin_pad_struct *)data;
    caps = gst_pad_get_caps(newpad);
    str = gst_caps_get_structure(caps,0);
#ifdef _DEBUG_	
    g_print("ma quanti nuovi pads: %s\n", gst_structure_get_name(str));
#endif
    /* we look at caps to see if an audio or video pad
     * has been just created 
     */
    if(g_strrstr(gst_structure_get_name(str),"audio")){
        gst_caps_unref(caps);
#ifdef _DEBUG_
        g_print("wow, an audio pad!\n");
        g_print("numsrcpads %d\n", binpads->decelem->numsrcpads);
#endif
        /* link the new pad to the following element
         */
        gst_pad_link(newpad, 
                gst_element_get_static_pad(binpads->asinkel,"sink"));
    }
    else if(g_strrstr(gst_structure_get_name(str),"video")){
        gst_caps_unref(caps);
#ifdef _DEBUG_
        g_print("wow, a video pad!\n");
#endif
        /* for a video pad the link is to the ghost pad
         */
        //gst_ghost_pad_set_target(GST_GHOST_PAD(vsrcghost),newpad);
        gst_pad_link(newpad,
                gst_element_get_static_pad(binpads->vsinkel, "sink"));
    }
    else{
        g_print("we got some strange thing\n");
        gst_caps_unref(caps);
        return;
    }	
}

/* this callback prints just a warning */
void 
cb_removed_decoded_pad()
{
#ifdef _DEBUG_
    g_print("Removed pad from decoder\n");
#endif
}

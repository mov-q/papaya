#include "gstmisc_elems.h"
/* 
 * used to create the filesrc element it must be fixed to use the 
 * global playlist counter for the location attribute
 */
GstElement* 
get_file_source(const char *fname)
{
    GstElement *ftemp;
    static int track;
    globconf_t *conf = parsed_conf;

    /* this is temporary code and will be probably 
     * be deprecated by the introduction of the read-ahead
     * track code
     */
#ifdef _DEBUG_
    print_playlist();
#endif	
    if(conf->playlist_mode == SHUFFLE){
        srand(time(NULL) + track);
        track = rand() % m_pl.p_tracks;
        m_pl.p_current = track;
    }
    else if (conf->playlist_mode == SEQUENTIAL){
        m_pl.p_current = (m_pl.p_current < m_pl.p_tracks-1) ? ++m_pl.p_current : 0;
    }
    /* create element and set location uri */
    ftemp = gst_element_factory_make("filesrc", "fsource");
    if(fname == NULL){

#ifdef _DEBUG_
        g_print("Now playing track [%d] - %s \n", m_pl.p_current,
                m_pl.p_element[m_pl.p_current]->file);
#endif
        g_object_set(G_OBJECT(ftemp), "location",
                m_pl.p_element[m_pl.p_current]->file, NULL);

    } else{
        g_print("Trying to add %s\n", fname);
        g_object_set(G_OBJECT(ftemp), "location", fname, NULL);
    }


    return ftemp;
}

GstElement*
get_dvb_source (void) {
    GstElement *dvbsrc;

    dvbsrc = gst_element_factory_make("dvbsrc", NULL);
    g_object_set(G_OBJECT(dvbsrc), "frequency", 754000000, NULL);
    g_object_set(G_OBJECT(dvbsrc), "bandwidth", 0, NULL);

    return dvbsrc;
};

GstElement*
get_dv1394_source (void) {
    GstElement *dv1394src;
    GstElement *dvq;
    GstElement *dvbin;
    GstPad *srcpad;

    dvbin = gst_bin_new("dv1394bin");

    dv1394src = gst_element_factory_make("dv1394src", NULL);
    //g_object_set(G_OBJECT(dv1394src), "guid", 0x0020110101002e47, NULL);
    g_object_set(G_OBJECT(dv1394src), "use-avc", FALSE, NULL);
    g_object_set(G_OBJECT(dv1394src), "do-timestamp", FALSE, NULL);
    gst_bin_add(GST_BIN(dvbin), dv1394src);

    dvq = gst_element_factory_make("queue2", "dv1394_read");
    gst_bin_add(GST_BIN(dvbin), dvq);
    gst_element_link(dv1394src, dvq);

    srcpad = gst_ghost_pad_new("src",
            gst_element_get_static_pad(dvq, "src"));
    gst_element_add_pad(dvbin, srcpad);

    return dvbin;
};

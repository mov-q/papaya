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
#include <streamer.h>


GstElement *pipeline = NULL;
GstClock *pipeclock = NULL;

static gboolean 
state_change(GstElement *newbin, guint seconds)
{
    gboolean kick;
    GstStateChangeReturn chret;
    GstClockTime base;
    GstClockTime now;
    GstClockTime offset;

    now = gst_clock_get_time(pipeclock);
    base = gst_element_get_base_time(newbin);
    offset = now -base;

    /* change state and evaluate return */
    chret = gst_element_set_state(newbin, GST_STATE_PAUSED);

    switch (chret) {
        case GST_STATE_CHANGE_NO_PREROLL:
            /* live source, timestamps are running_time of the
             * pipeline clock. */
            g_printf("no preroll\n");
            kick = FALSE;
            break;
        case GST_STATE_CHANGE_SUCCESS:
            /* success, no async state changes,
             * same as async, timestamps start
             *        * from 0 */
            g_printf("changed immediately\n");
        case GST_STATE_CHANGE_ASYNC:
            /* no live source, bin will preroll.
             * We have to punch it in because in
             * this situation timestamps
             *        start from 0.  */
            g_printf("will change async\n");
            kick = TRUE;
            break;
        default:
        case GST_STATE_CHANGE_FAILURE:
            return FALSE;
    }

    if(seconds)
        g_usleep(seconds * G_USEC_PER_SEC);

    if(kick){
        now = gst_clock_get_time(pipeclock);
        base = now - offset;

        gst_element_set_base_time(newbin, base);
    }
}

static void 
cb_change_track(gpointer *mdata)
{
    struct srcbin_pad_struct *cbs;
    GstElement *ns;

    cbs = (struct srcbin_pad_struct *) mdata;
    /* first get rid of the old element */
    gst_element_set_state(cbs->mbin, GST_STATE_READY);
    gst_element_unlink(cbs->srcelem, cbs->decelem);
    gst_element_set_state(cbs->srcelem, GST_STATE_NULL);
    gst_bin_remove(GST_BIN(cbs->mbin),cbs->srcelem);

    /* allocate the new element and restart
     * the whole bin 
     */ 
    cbs->srcelem = get_file_source(NULL);
    gst_bin_add(GST_BIN(cbs->mbin), cbs->srcelem);
    gst_element_link(cbs->srcelem, cbs->decelem);
    gst_element_set_state(cbs->mbin, GST_STATE_PLAYING);

}

static void 
rt_stop_single_file(gpointer *mdata)
{
    struct input_source *tr;

    tr = (struct input_source *) mdata;
    g_print("%p\n", tr->srcptr);
    remove_stream_source(tr);
}

static gboolean 
cb_stop_single_file(GstPad *pad, 
                    GstEvent *te, 
                    gpointer u_data)
{

    switch GST_EVENT_TYPE(te){
        case GST_EVENT_EOS:
        {
#ifdef _DEBUG_
            g_print("EOS in additional src\n");
#endif
            g_thread_create((GThreadFunc)rt_stop_single_file, 
                            u_data, TRUE, NULL);
            return FALSE;
        }
        default:
        {
            return TRUE;
        }
    }

};


void 
_cmd_add_net(const char *url)
{
    GstElement *ns;
    GstElement *tprobe;
    struct input_source *net_in;

    ns = get_net_source_bin(url);
    /* this probes for EOS */
    tprobe = gst_bin_get_by_name(GST_BIN(ns), "netsrc");
    gst_bin_add(GST_BIN(pipeline), ns);

    net_in = add_new_stream_source(ns, "asrc%d", url );
    net_in->ep_handler = gst_pad_add_event_probe(
            gst_element_get_static_pad(tprobe,"src"), 
            G_CALLBACK(net_event), net_in);
    gst_element_set_state(ns, GST_STATE_PLAYING);	
    net_in->pprobed = gst_element_get_static_pad(tprobe, "src");
}

void 
_cmd_add_audio_live(const char *device)
{
    GstElement *al;

    al = get_alsa_source_bin();
    gst_bin_add(GST_BIN(pipeline), al);

    add_new_stream_source(al, "asrc%d", "alsa live source");

    gst_element_set_state(al, GST_STATE_PLAYING);

};

void 
_cmd_add_audio_file(const struct typed_command *lcom)
{
    GstElement *nfile;
    struct input_source *nsrc;
    GstPad *probepad;
    struct stat *finfo;

    finfo = (struct stat *)malloc(sizeof(struct stat));
    if(0 == stat(lcom->cmd_argv[1], finfo)){
        nfile = get_source_bin(lcom->cmd_argv[1]);
        gst_bin_add(GST_BIN(pipeline), nfile);
        probepad = gst_element_get_static_pad(nfile, "asrc%d");

        nsrc = add_new_stream_source(nfile, "asrc%d", 
                "additional file source");
        //nsrc->ep_handler = gst_pad_add_event_probe(
        nsrc->ep_handler = gst_pad_add_event_probe(probepad, 
                G_CALLBACK(cb_stop_single_file), nsrc);
        gst_element_set_state(nfile, GST_STATE_PLAYING);
        nsrc->pprobed = probepad;
    }
    else{
        g_printf("File does not exist!\n");
    }
    free(finfo);
};

static gboolean 
cb_papaya_pipeline_bus(GstBus *streambus,
                       GstMessage *msg, 
                       gpointer data)
{

    switch (GST_MESSAGE_TYPE(msg)){
        case GST_MESSAGE_ERROR: 
            {
                GError *err = NULL;
                gchar *debug = NULL;

                gst_message_parse_error (msg, &err, &debug);
                gst_object_default_error (GST_MESSAGE_SRC(msg), err, debug);
                g_print("\n");
                g_error_free(err);
                if (debug)
                    g_free(debug);
            }
        case GST_MESSAGE_WARNING: 
            {
                /*
                   g_print("***WARNING*** !");
                   GError *err;
                   gchar *debug;

                   gst_message_parse_warning(msg, &err, &debug);
                   g_print("Warning: %s\n", err->message);
                   g_error_free(err);
                   if(debug)
                   g_free(debug);
                 */
            }
        case GST_MESSAGE_EOS: 
            {
                /* do nothing, handled elsewhere */
            }
        default:
            break;
    }

    return TRUE;
};


GstElement* 
build_audio_output_chain()
{
    GstElement *abin;
    GstElement *aconv;
    GstElement *asample;
    GstElement *conv;
    GstElement *qstream;
    GstElement *prev, *next;
    GstElement *audiosampfilter;
    GstElement *audiofilter;

    GstCaps *acaps;
    GstCaps *sampcapsi;
    GstCaps *sampcapsf;

    gchar capstring[MAX_CAPS_STRING_SIZE];

    GstPad *audiopad;
    globconf_t *conf = parsed_conf;

    /* create audio output */
    abin = gst_bin_new ("audiobin");

    qstream = gst_element_factory_make("queue2", "audioenc_queue");
    gst_bin_add(GST_BIN(abin), qstream);
    audiopad = gst_element_get_static_pad(qstream,"sink");
    prev = qstream;

    aconv = gst_element_factory_make("audioconvert","aconv");
    next = aconv;
    gst_bin_add(GST_BIN(abin),aconv);
    gst_element_link(prev,next);
    prev = next;
    
    /* the following code must be fixed to get resampling working */
    asample = gst_element_factory_make("audioresample", "asample");
    next = asample;
    //audiopad = gst_element_get_static_pad(asample, "sink");
    gst_bin_add(GST_BIN(abin), asample);
    gst_element_link(prev,next);
    prev = next;


    if(conf->acodec == MP3){

        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/mpeg, rate=%d, channels=%d", 
                conf_get_audiosettings_samplerate(),
                conf_get_audiosettings_channels());

        acaps = gst_caps_from_string(capstring);

        next = conv = get_mp3_encoder();
        gst_bin_add(GST_BIN(abin),conv);
        gst_element_link(prev,next);
        prev = next;


        audiofilter = gst_element_factory_make("capsfilter", "lamecaps");
        g_object_set(G_OBJECT(audiofilter), "caps", acaps, NULL);
        next = audiofilter;
        gst_bin_add(GST_BIN(abin), audiofilter);
        gst_element_link(prev, next);
        prev = next;
    }
    else if(conf->acodec == OGG){

        next = conv = get_ogg_encoder();
        gst_bin_add(GST_BIN(abin),conv);
        gst_element_link(prev,next);
        prev = next;

        next = audiofilter = gst_element_factory_make("capsfilter", "vorbiscaps");
        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/x-vorbis, rate=%d, channels=%d", 
                conf_get_audiosettings_samplerate(),
                conf_get_audiosettings_channels());
        acaps = gst_caps_from_string(capstring);
        g_object_set(G_OBJECT(audiofilter), "caps", acaps, NULL);
        gst_bin_add(GST_BIN(abin), next);
        gst_element_link(prev, next);
        prev = next;

        next = get_ogg_mux();
        gst_bin_add(GST_BIN(abin),next);
        gst_element_link(prev,next);
        prev = next;
    }
    else if(conf->acodec == AAC){

        g_snprintf(capstring, MAX_CAPS_STRING_SIZE, 
                "audio/mpeg, rate=96000, channels=%2", 
                conf_get_audiosettings_samplerate(),
                conf_get_audiosettings_channels());

        acaps = gst_caps_from_string(capstring);

        next = conv = get_aac_encoder();
        gst_bin_add(GST_BIN(abin),conv);
        gst_element_link(prev,next);
        prev = next;

        /*
        audiofilter = gst_element_factory_make("capsfilter", "aaccaps");
        g_object_set(G_OBJECT(audiofilter), "caps", acaps, NULL);
        next = audiofilter;
        gst_bin_add(GST_BIN(abin), audiofilter);
        gst_element_link(prev, next);
        prev = next;*/
    }

    gst_element_add_pad (abin,gst_ghost_pad_new ("sink", audiopad));
    gst_element_add_pad(abin, 
            gst_ghost_pad_new("src", 
                gst_element_get_static_pad(next, "src")));

    gst_object_unref (audiopad);

    return abin;
}

GstElement* 
get_xv_bin()
{
    GstElement *tempxbin;
    GstElement *vqueue;
    GstElement *xvsink;
    GstPad *vqpad;

    tempxbin = gst_bin_new("xvpipe");

    vqueue = gst_element_factory_make("queue2", "xvout_queue");
    vqpad = gst_element_get_static_pad(vqueue, "sink");
    gst_bin_add(GST_BIN(tempxbin), vqueue);

    xvsink = gst_element_factory_make("xvimagesink", "xvsink");
    gst_bin_add(GST_BIN(tempxbin), xvsink);

    gst_element_link(vqueue, xvsink);

    gst_element_add_pad(tempxbin,gst_ghost_pad_new("sink",vqpad));

    return tempxbin;

}


static GstElement* 
get_schroedinger_bin()
{
    GstElement *schroebin;

    GstElement *vqueue;
    GstElement *convspace;
    GstElement *schroenc;
    GstElement *tover;
    GstElement *vrate;
    GstElement *vscale;
    GstElement *vfilter;
    GstElement *vtee;

    GstPad *tbinsink;
    GstPad *tbinsrc;

    schroebin = gst_bin_new(NULL);

    vqueue = gst_element_factory_make("queue2", "schroenc_queue");
    gst_bin_add(GST_BIN(schroebin), vqueue);
    tbinsink = gst_element_get_static_pad(vqueue, "sink");
    
    vtee = gst_element_factory_make("tee", NULL);
    gst_bin_add(GST_BIN(schroebin), vtee);
    gst_element_link(vqueue, vtee);

    convspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(GST_BIN(schroebin), convspace);
    gst_element_link(vqueue, convspace);

    vrate = gst_element_factory_make("videorate", NULL);
    gst_bin_add(GST_BIN(schroebin), vrate);
    gst_element_link(convspace,vrate);

    vscale = get_video_scale();
    gst_bin_add(GST_BIN(schroebin), vscale);
    gst_element_link(vrate, vscale);

    vfilter = get_video_scale_capsfilter();
    gst_bin_add(GST_BIN(schroebin), vfilter);
    gst_element_link(vscale, vfilter);

    tover = gst_element_factory_make("textoverlay", NULL);
    g_object_set(G_OBJECT(tover), "text", "Papaya broadcaster", NULL);
    g_object_set(G_OBJECT(tover), "halignment", 0, NULL);
    g_object_set(G_OBJECT(tover), "shaded-background", TRUE, NULL);
    gst_bin_add(GST_BIN(schroebin), tover);
    gst_element_link(vfilter,tover);

    schroenc = gst_element_factory_make("schroenc", NULL);
    tbinsrc = gst_element_get_static_pad(schroenc, "src");
    g_object_set(G_OBJECT(schroenc), "bitrate", conf_get_theora_encoder_bitrate(), NULL);
    gst_bin_add(GST_BIN(schroebin), schroenc);
    gst_element_link(tover, schroenc);

    gst_element_add_pad(schroebin, gst_ghost_pad_new("sink", tbinsink));
    gst_element_add_pad(schroebin, gst_ghost_pad_new("src", tbinsrc));

    gst_object_unref(tbinsrc);
    gst_object_unref(tbinsink);

    return schroebin;

}



/* activate the alsasink output (audiomonitor) */
void 
pass_on()
{
    GstElement *falsa;

    falsa = get_alsa_out();
    gst_bin_add_many(GST_BIN(pipeline), falsa, NULL);
    add_new_outputs(falsa, "sink", "an alsa output");
}


void 
_cmd_remove_source(int srcpos)
{
    GList *current_list;
    struct input_source *tr;

    current_list = globsrc->sources_l;
    tr = (struct input_source *) g_list_nth_data(current_list, srcpos);

    remove_stream_source(tr);
}

void 
_cmd_remove_output(int srcpos)
{
    GList *current_list;
    struct pipe_out *pout;

    current_list = glob_outputs->out_list;
    pout = (struct pipe_out *) g_list_nth_data(current_list, srcpos);

    remove_stream_output(pout);
}

/* mute a given source
 * the source is identified by its position id
 */
void 
_cmd_mute(int srcpos)
{
    GList *current_list;
    GstElement *vol;
    struct input_source *tr;

    /* get the source struct by position */
    current_list = globsrc->sources_l;
    tr = (struct input_source *)g_list_nth_data(current_list, srcpos);

    vol = gst_bin_get_by_name(GST_BIN(tr->srcptr), "srcvolume");

    g_object_set(G_OBJECT(vol), "mute", TRUE, NULL);

}

void 
_cmd_pass_on()
{
    GstPad *teepad;
    GstPad *falsapad;
    GstElement *falsa;

    falsa = get_alsa_out();
    gst_bin_add(GST_BIN(pipeline), falsa);
    add_new_outputs(falsa, "sink", "buggy alsa added");
    g_printf("THIS IS BUGGY AND SILENTLY CRASHES PAPAYA!!\n");
    /* FIXME: let it work
     * */
    gst_element_set_state(falsa, GST_STATE_PAUSED);
    g_usleep(4000);
    gst_element_set_state(falsa, GST_STATE_PLAYING);
}

/* unmute a given source
 * the source is identified by its position id
 */
void 
_cmd_unmute(int srcpos)
{
    GList *current_list;
    GstElement *vol;
    struct input_source *tr;

    /* get the source struct by position */
    current_list = globsrc->sources_l;
    tr = (struct input_source *)g_list_nth_data(current_list, srcpos);

    vol = gst_bin_get_by_name(GST_BIN(tr->srcptr), "srcvolume");

    g_object_set(G_OBJECT(vol), "mute", FALSE, NULL);

}

/* put the volume of the given source 
 * up by one unit
 */ 
int 
_cmd_volup(int srcpos)
{
    GList *current_list;
    GstElement *vol;
    struct input_source *tr;
    gdouble cur_vol;

    /* get the source struct by position */
    current_list = globsrc->sources_l;
    tr = (struct input_source *)g_list_nth_data(current_list, srcpos);
    /* get the volume element from the source */
    vol = gst_bin_get_by_name(GST_BIN(tr->srcptr), "srcvolume");

    g_object_get(G_OBJECT(vol), "volume", &cur_vol, NULL);
    g_print("vol: %f\n", cur_vol);
    if(cur_vol >= 10){
        return 0;
    }

    cur_vol = cur_vol + 0.2;
    g_object_set(G_OBJECT(vol), "volume", cur_vol, NULL);

    return 1;
}

/* put the volume of the given source 
 * up by one unit
 */ 
int 
_cmd_voldown(int srcpos)
{
    GList *current_list;
    GstElement *vol;
    struct input_source *tr;
    gdouble cur_vol;

    /* get the source struct by position */
    current_list = globsrc->sources_l;
    tr = (struct input_source *)g_list_nth_data(current_list, srcpos);
    /* get the volume element from the source */
    vol = gst_bin_get_by_name(GST_BIN(tr->srcptr), "srcvolume");

    g_object_get(G_OBJECT(vol), "volume", &cur_vol, NULL);
    g_print("vol: %f\n", cur_vol);
    if(cur_vol <= 0){
        return 0;
    }

    cur_vol = cur_vol - 0.2;
    g_object_set(G_OBJECT(vol), "volume", cur_vol, NULL);

    return 1;
}



static void 
cb_change_vtrack(gpointer mdata)
{
    struct av_source *avinput;
    struct input_source *avain;
    struct input_source *avvin;

    avinput = (struct av_source *) mdata;
    remove_vstream_source(avinput);
    _cmd_add_vpl();
}


static gboolean 
cb_eos_in_videopl_astream(GstPad *pad, 
                          GstEvent *te, 
                          gpointer u_data)
{
    GstElement *newtemp;
    switch GST_EVENT_TYPE(te){
        case GST_EVENT_EOS:{
            g_print("audio EOS in astream\n");
            return FALSE;
        }
        default:{
            /* 
             * returning true does not stop handling of other events 
             */
            return TRUE;
        }
    }
}

static gboolean 
cb_eos_in_videopl_vstream(GstPad *pad, 
                          GstEvent *te, 
                          gpointer u_data)
{
    GstElement *newtemp;
    switch GST_EVENT_TYPE(te){
        case GST_EVENT_EOS:{
            g_print("video EOS in filesrc\n");
            g_thread_create((GThreadFunc)cb_change_vtrack, 
                            u_data, TRUE, NULL);
            return FALSE;
        }
        default:{
            /* returning true does not stop handling of other events */
            return TRUE;
        }
    }
}

static void 
cb_decoder_drained(GstElement *dbin, gpointer u_data)
{

    g_print("Drained!!\n");
    g_thread_create((GThreadFunc)cb_change_vtrack, u_data, TRUE, NULL);
}

void 
_cmd_add_vpl()
{
    GstElement *vns;
    GstElement *tprobe;
    struct av_source *avs;

    avs = (struct av_source *)malloc(sizeof(struct av_source));

    vns = get_source_bin(NULL);

    gst_bin_add(GST_BIN(pipeline), vns);
    avs->vstruct = add_new_vstream_source(vns, 
            "vsrc", 
            "new video source");
    avs->astruct = add_new_stream_source(vns, 
            "asrc%d", 
            "new audio source");

    avs->astruct->pprobed = gst_element_get_static_pad(vns, "asrc%d");
    avs->astruct->ep_handler = gst_pad_add_event_probe(
            gst_element_get_static_pad(vns, "asrc%d"), 
            G_CALLBACK(cb_eos_in_videopl_astream), NULL);
    avs->vstruct->pprobed = gst_element_get_static_pad(vns, "vsrc");
    avs->vstruct->ep_handler = gst_pad_add_event_probe(
            gst_element_get_static_pad(vns, "vsrc"), 
            G_CALLBACK(cb_eos_in_videopl_vstream), avs);

    gst_element_set_state(vns, GST_STATE_PLAYING);
    g_print("numsinkpads gvideomixer - adder: %d <-> %d\n", 
            gvmixer->numsinkpads, adder->numsinkpads);

}

void 
_cmd_add_box()
{
    GstElement *newbox;
    struct input_source *vbstruct;

    newbox = get_videobox_source_bin();
    gst_bin_add(GST_BIN(pipeline), newbox);

    vbstruct = add_new_vbox_source(newbox, "vsrc", "new box source");

    gst_element_set_state(newbox, GST_STATE_PLAYING);
}

void 
papaya_audio_tree_build()
{
    GstElement *audio;
    GstElement *sout;

    GstPad *prevpad;
    struct input_source *in_src;
    /* 
     * we handle audio chains for audio live and 
     * playlist here
     */
    switch (STREAM_TYPE){
        case AUDIO_PLAYLIST:
            srcbin = get_source_bin(NULL);
            break;
        case AUDIO_LIVE:
            srcbin = get_alsa_source_bin();
            break;
    }

    gst_bin_add(GST_BIN (pipeline), srcbin);

    silence = get_silence_bin();
    gst_bin_add(GST_BIN(pipeline), silence);

    /* 
     * this is the new code to add sources and link them 
     * to adder and in a list of sources 
     */ 	
    adderbin = get_adder_bin();	
    gst_bin_add(GST_BIN(pipeline), adderbin);

    /* add sources */
    add_new_stream_source(silence, "src", "main silence source");
    in_src = add_new_stream_source(srcbin, 
            "asrc%d", "bootstrap playlist source");
    /* this is used for the playlist EOS handling */
    gst_pad_add_event_probe(gst_element_get_static_pad(srcbin,"asrc%d"), 
            G_CALLBACK(cb_eos_in_filesrc), in_src);

    g_print("active sources %d\n", globsrc->active);

    atee = gst_element_factory_make("tee","tee");
    gst_bin_add(GST_BIN(pipeline),atee);
    gst_element_link(adderbin, atee);

    /*
     * build output is the encoders and converter chain
     */
    audio = build_audio_output_chain();	
    gst_bin_add (GST_BIN (pipeline), audio);

    /* the shout2send element */
    sout = get_shout_out();
    gst_bin_add(GST_BIN(pipeline), sout);
    gst_element_link(audio, sout);


    add_new_outputs(audio, "sink", "a shout2send output");

    /* activate the audio monitor if requested */
    if(audiomon)
        pass_on();	
}


static void 
papaya_video_tree_build()
{
    GstElement *apq, *vpq;
    GstElement *avorb;
    GstElement *vshoutsend;
    GstElement *vtheora;
    GstElement *cbprobe;
    GstElement *generalout;

    globconf_t *conf = parsed_conf;

    struct av_source *av;
    /* video playlist, video live, xorg live
     * streams are handled here 
     */ 	
    if(STREAM_TYPE == VIDEO_PLAYLIST ||
       STREAM_TYPE == DVB_LIVE ||
       STREAM_TYPE == DV1394_LIVE)
        srcbin = get_source_bin(NULL);
    else if(STREAM_TYPE == VIDEO_LIVE)
        srcbin = get_av_live_source_bin();
    else if(STREAM_TYPE == XORG_LIVE)	
        srcbin = get_x_source_bin();

    /* add the source bin to the pipeline
     */ 
    CHECK(gst_bin_add(GST_BIN(pipeline), srcbin));

    /* in case of video streaming, audio is encoded in 
     * vorbis format and video in theora format
     * TODO: for both video and audio we should have a tee element
     * to which attaching monitors
     */
    gvmixer = gst_element_factory_make("videomixer", "mainvmixer");
    CHECK(gst_bin_add(GST_BIN(pipeline), gvmixer));

    adderbin = get_adder_bin();	
    CHECK(gst_bin_add(GST_BIN(pipeline), adderbin));

    
    if(STREAM_TYPE == VIDEO_PLAYLIST)
    {
        GstElement *tvid;
        tvid = get_videotest_bin();
        gst_bin_add(GST_BIN(pipeline), tvid);
        add_new_vstream_source(tvid, "tsrc", "testvid");
    }

    av = (struct av_source *)malloc(sizeof(struct av_source));
    av->vstruct = add_new_vstream_source(srcbin, "vsrc", "mainvideo");
    av->astruct = add_new_stream_source(srcbin, "asrc%d", "mainaudio");
    gst_pad_add_event_probe(gst_element_get_static_pad(srcbin, "asrc%d"),
            G_CALLBACK(cb_eos_in_videopl_astream), NULL);
    cbprobe = gst_bin_get_by_name(GST_BIN(srcbin), "fdecode");
    gst_pad_add_event_probe(gst_element_get_static_pad(srcbin, "vsrc"), 
            G_CALLBACK(cb_eos_in_videopl_vstream), av);
#ifdef _DEBUG_
    g_print("mainvid: %s", av->vstruct->req_pad_name);
#endif
    apq = gst_element_factory_make("queue2", NULL);
    gst_bin_add(GST_BIN(pipeline), apq);
    gst_element_link(adderbin, apq);

    vpq = gst_element_factory_make("queue2", NULL);
    gst_bin_add(GST_BIN(pipeline), vpq);
    gst_element_link(gvmixer, vpq);

    if(conf->serving_mode == ICECAST_SERV_MODE){
        //FIXME: quite horrid audio chain here must be fixed
        avorb = get_vorbis_enc();
        gst_bin_add(GST_BIN(pipeline), avorb);
        gst_element_link(apq, avorb);

        vtheora = get_theora_bin();
        gst_bin_add(GST_BIN(pipeline), vtheora);
        gst_element_link(vpq, vtheora);

        /* the following element is a bin made of a 
         * oggmux element and the shout2send element */	
        vshoutsend = get_shout_video_out();
        gst_bin_add(GST_BIN(pipeline), vshoutsend);

        gst_pad_link(gst_element_get_static_pad(avorb, "src"),
                gst_element_get_static_pad(vshoutsend, "sink_0"));

        gst_pad_link(gst_element_get_static_pad(vtheora, "src"),
                gst_element_get_static_pad(vshoutsend, "sink_1"));
    }
    else if (conf->serving_mode == RTP_SERV_MODE){

        generalout = get_rtp_output_pipe();
        gst_bin_add(GST_BIN(pipeline), generalout);

        gst_pad_link(gst_element_get_static_pad(apq, "src"),
                    gst_element_get_static_pad(generalout, "asink"));
        gst_pad_link(gst_element_get_static_pad(vpq, "src"),
                gst_element_get_static_pad(generalout, "vsink"));

    }
}


/* this is the first function where pipeline is built */
void 
setup_streamer(int narg, char *sarg[])
{
    /* the message bus */
    GstBus *bus;

    /* this init the global sources structures */
    init_global_sources();
    init_global_outputs();

    pipeline = gst_pipeline_new("pipeline");
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, cb_papaya_pipeline_bus, loop);
    gst_object_unref (bus);

    switch (STREAM_TYPE){
        case AUDIO_PLAYLIST:
        case AUDIO_LIVE:
            papaya_audio_tree_build();
            break;
        case VIDEO_PLAYLIST:
        case VIDEO_LIVE:
        case XORG_LIVE:
        case DVB_LIVE:
        case DV1394_LIVE:
            papaya_video_tree_build();
            break;
    }
    /* after having built the chain, let's
     * play some media */ 
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    pipeclock = gst_element_get_clock(pipeline);
}


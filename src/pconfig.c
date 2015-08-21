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
#include <pconfig.h>

void 
papaya_config_parse (void)
{
    xmlXPathObjectPtr xpathObj = NULL;
    xmlNodeSetPtr nodi;

    if(!configfile){
        g_print("You must specify a configuration file!\n");
        exit(1);
    }

    if(!(xml_doc = xmlParseFile(configfile))){
        g_print("An error occured while parsing the configuration file\n");
        exit(1);
    }

    xpath_ctx = xmlXPathNewContext(xml_doc);

};

enum serv_mode 
conf_get_server_type (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/server-type", xpath_ctx);   
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "icecast"))
            return ICECAST_SERV_MODE;
        else if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "rtp"))
            return RTP_SERV_MODE;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Server must be \"icecast\" or \"rtp\"\n");
    exit(-1);
};

xmlChar* 
conf_get_ice_server (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/ice-0/server", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return retstr;
    }
    g_printf("Check your configuration!!\n");
    g_printf("You must supply the hostname of the Icecast server\n");
    exit(-1);
};

int 
conf_get_ice_port (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;
    short int iceport;

    xres = xmlXPathEvalExpression("/config/ice-0/port", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr)> 0){
            if((iceport = (short int)atoi(retstr)) > 0)
                return atoi(retstr);
        }
    }

    g_printf("Check your configuration!!\n");
    g_printf("You must supply the port of the Icecast server\n");
    exit(-1);

};

int 
conf_get_ice_format (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/ice-0/format", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "mp3"))
            return 0;
        else if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "ogg"))
            return 1;
        else if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "aac"))
            return 2;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Icecast format must be \"mp3\" or \"ogg\"\n");
    exit(-1);
};


xmlChar* 
conf_get_ice_username (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/ice-0/username", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return retstr;
    }

    g_printf("Check your configuration!!\n");
    g_printf("You must supply the username of the icecast account\n");
    exit(-1);
};

xmlChar* 
conf_get_ice_password (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/ice-0/password", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return retstr;
    }

    g_printf("Check your configuration!!\n");
    g_printf("You must supply the password of the icecast account\n");
    exit(-1);
};

xmlChar* 
conf_get_ice_mountpoint (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/ice-0/mountpoint", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return retstr;
    }

    g_printf("Check your configuration!!\n");
    g_printf("You must supply the icecast mountpoint\n");
    exit(-1);
};

xmlChar* 
conf_get_ice_streamname (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/ice-0/streamname", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

xmlChar* 
conf_get_ice_description (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/ice-0/description", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

xmlChar* 
conf_get_ice_genre (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/ice-0/genre", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

xmlChar* 
conf_get_rtp_host (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/rtp-0/host", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return retstr;
    }
    g_printf("Check your configuration!!\n");
    g_printf("You must supply the hostname of the Icecast server\n");
    exit(-1);
};

int 
conf_get_rtp_port (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;
    short int iceport;

    xres = xmlXPathEvalExpression("/config/rtp-0/port", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr)> 0){
            if((iceport = (short int)atoi(retstr)) > 0)
                return atoi(retstr);
        }
    }

    g_printf("Check your configuration!!\n");
    g_printf("You must supply the port of the rtp host\n");
    exit(-1);

};

int 
conf_get_rtp_video_format (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/rtp-0/video-format", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "h264"))
            return 0;
        else if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "mpeg4"))
            return 1;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Video format must be \"h264\" or \"mpeg4\"\n");
    exit(-1);
};

int 
conf_get_rtp_audio_format (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/rtp-0/audio-format", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "mp3"))
            return 0;
        else if(0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]), "ogg"))
            return 1;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Rtp audio format must be \"mp3\" or \"ogg\"\n");
    exit(-1);
};

int 
conf_get_mp3_encoder_bitrate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/mp3-encoder/bitrate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_mp3_encoder_quality (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/mp3-encoder/quality", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_vorbis_encoder_bitrate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/vorbis-encoder/bitrate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return (1000 * atoi(xmlNodeGetContent(nset->nodeTab[0])));
    }

    return -1;
};

enum encoder_type 
conf_get_vorbis_encoder_type (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/vorbis-encoder/type", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "quality"))
            return QUALITY;
        else if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "bitrate"))
            return BITRATE;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Encoder type can be \"quality\" or \"bitrate\"\n");
    exit(-1);
};


int 
conf_get_vorbis_encoder_samplerate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/vorbis-encoder/samplerate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

gfloat 
conf_get_vorbis_encoder_quality (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/vorbis-encoder/quality", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return (gfloat)atof(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_audiosettings_channels (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/audiosettings/channels", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};


int 
conf_get_audiosettings_samplerate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/audiosettings/samplerate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};


xmlChar* 
conf_get_videosettings_framerate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/videosettings/framerate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

int 
conf_get_videosettings_width (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/videosettings/width", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_videosettings_height (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/videosettings/height", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

xmlChar* 
conf_get_live_video_device (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/live-video/device", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

int 
conf_get_x_grab_startx (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/x-grab/startx", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_x_grab_starty (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/x-grab/starty", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};


int 
conf_get_x_grab_endx (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/x-grab/endx", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_x_grab_endy (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/x-grab/endy", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

enum encoder_type 
conf_get_theora_encoder_type (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/theora-encoder/type", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "quality"))
            return QUALITY;
        else if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "bitrate"))
            return BITRATE;
    }

    g_printf("Check your configuration!!\n");
    g_printf("Theora encoder type can be \"quality\" or \"bitrate\"\n");
    exit(-1);
};

int 
conf_get_theora_encoder_bitrate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/theora-encoder/bitrate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};


int 
conf_get_theora_encoder_quality (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/theora-encoder/quality", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

enum encoder_type 
conf_get_h264_encoder_type (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/h264-encoder/type", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "quality"))
            return QUALITY;
        else if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "bitrate"))
            return BITRATE;
        else if (0 == strcmp(xmlNodeGetContent(nset->nodeTab[0]),
                    "quantizer"))
            return QUANTIZER;
    }

    g_printf("Check your configuration!!\n");
    g_printf("H264 encoder type can be \"quality\" or \"bitrate\"\n");
    exit(-1);
};

int 
conf_get_h264_encoder_bitrate (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/h264-encoder/bitrate", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

int 
conf_get_h264_encoder_quality (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/h264-encoder/quality", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return atoi(xmlNodeGetContent(nset->nodeTab[0]));
    }

    return -1;
};

xmlChar* 
conf_get_live_audio_device (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/live-audio/alsa/alsa-device", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

gint64 
conf_get_live_audio_bufutime (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/live-audio/alsa/buffer-utime", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return (gint64)atoi(retstr);
    }
    /* return gstreamer default value */
    return 20000;
};

gint64 
conf_get_live_audio_lattime (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/live-audio/alsa/latency-time", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return (gint64)atoi(retstr);
    }

    /* return the gstreamer default value */
    return 10000;
};

xmlChar* 
conf_get_live_pulse_device (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;

    xres = xmlXPathEvalExpression("/config/live-audio/pulse/pulse-device", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        return xmlNodeGetContent(nset->nodeTab[0]);
    }

    return NULL;
};

gint64 
conf_get_live_pulse_latency_time (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/live-audio/pulse/latency-time", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return (gint64)atoi(retstr);
    }

    /* return the gstreamer default value */
    return 500000;
};


gint64 
conf_get_live_pulse_buffer_time (void)
{
    xmlXPathObjectPtr xres;
    xmlNodeSetPtr nset;
    xmlChar *retstr;

    xres = xmlXPathEvalExpression("/config/live-audio/pulse/buffer-time", xpath_ctx);
    nset = xres->nodesetval;
    if(!xmlXPathNodeSetIsEmpty(nset)){
        retstr = xmlNodeGetContent(nset->nodeTab[0]);
        if(strlen(retstr) > 0)
            return (gint64)atoi(retstr);
    }

    /* return the gstreamer default value */
    return 10000;
};

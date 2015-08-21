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
#include "confreader.h"

globconf_t *parsed_conf = NULL;

static void config_struct_init(){
    /* init xml context and get ready for
     * further parsing
     */
    papaya_config_parse();
    /* allocation of config variables and structures */
    globconf_t *p;
    parsed_conf = (globconf_t *)malloc(sizeof(globconf_t));	
    p = parsed_conf;

    p->ice_server = (struct p_ice_config *)
        malloc(sizeof(struct p_ice_config));
    
    p->serving_mode = conf_get_server_type();
    /* get the icecast configuration*/
    p->ice_server->hostname = conf_get_ice_server();
    p->ice_server->port = conf_get_ice_port();
    p->ice_server->aformat = conf_get_ice_format();
    p->ice_server->username = conf_get_ice_username();
    p->ice_server->password = conf_get_ice_password();
    p->ice_server->mountpoint = conf_get_ice_mountpoint();
    p->ice_server->stream_name = conf_get_ice_streamname();
    p->ice_server->description = conf_get_ice_description();
    p->ice_server->genre = conf_get_ice_genre();
    /* replicate format setting here */
    p->acodec = p->ice_server->aformat;
    
    /* get the rtp configuration */
    p->rtp_server = (struct p_rtp_config *)
        malloc(sizeof(struct p_rtp_config));
    p->rtp_server->host = conf_get_rtp_host();
    p->rtp_server->port = conf_get_rtp_port();
    p->rtp_server->aformat = conf_get_rtp_audio_format();
    p->rtp_server->vformat = conf_get_rtp_video_format();

    /* mp3 encoder params */
    p->mp3_par = (struct p_mp3_params *) 
        malloc(sizeof(struct p_mp3_params));
    p->mp3_par->bitrate = conf_get_mp3_encoder_bitrate();
    p->mp3_par->quality = conf_get_mp3_encoder_quality();

    /* vorbis encoder params */
    p->vorbis_par = (struct p_vorbis_params *) 
        malloc(sizeof(struct p_vorbis_params));
    p->vorbis_par->type = conf_get_vorbis_encoder_type();
    p->vorbis_par->bitrate = conf_get_vorbis_encoder_bitrate();
    p->vorbis_par->quality = conf_get_vorbis_encoder_quality();
    
    /* h264 encoder params */
    p->h264_par = (struct p_h264_params *) 
        malloc(sizeof(struct p_h264_params));
    p->h264_par->type = conf_get_h264_encoder_type();
    p->h264_par->bitrate = conf_get_h264_encoder_bitrate();
    p->h264_par->quality = conf_get_h264_encoder_quality();

    /* live audio settings */
    p->live_audio_par = (struct p_live_audio *)
        malloc(sizeof(struct p_live_audio));
    /* alsa settings */
    p->live_audio_par->alsa_par = (struct p_alsa_params *)
        malloc(sizeof(struct p_alsa_params));
    struct p_alsa_params *apar = p->live_audio_par->alsa_par;
    apar->device = conf_get_live_audio_device();
    apar->buf_time = conf_get_live_audio_bufutime();
    apar->lat_time = conf_get_live_audio_lattime();

    /* live video settings */
    p->live_video_par = (struct p_live_video *)
        malloc(sizeof(struct p_live_video));
    struct p_live_video *lvpar = p->live_video_par;
    lvpar->device = conf_get_live_video_device();

    /* audiosettings */
    p->audiosettings = (struct p_audio_settings *)
        malloc(sizeof(struct p_audio_settings));
    struct p_audio_settings *asets = p->audiosettings;
    asets->samplerate = conf_get_audiosettings_samplerate();
    asets->channels = conf_get_audiosettings_channels();

    p->videosettings = (struct p_video_settings *)
        malloc(sizeof(struct p_video_settings));
    struct p_video_settings *vsets = p->videosettings;
    vsets->framerate = conf_get_videosettings_framerate();
    vsets->width = conf_get_videosettings_width();
    vsets->height = conf_get_videosettings_height();

    /* x grab settings */
    p->xgrabsettings = (struct x_grab_settings *)
        malloc(sizeof(struct x_grab_settings));
    struct x_grab_settings *xsets = p->xgrabsettings;
    xsets->startx = conf_get_x_grab_startx();
    xsets->starty = conf_get_x_grab_starty();
    xsets->endx = conf_get_x_grab_endx();
    xsets->endy = conf_get_x_grab_endy();


    /* other settings */
    if(SHUFFLE_MODE)
        p->playlist_mode = SHUFFLE;
    else
        p->playlist_mode = SEQUENTIAL;

    if(WITH_PULSE)
        p->audio_mode = PULSE;
    else
        p->audio_mode = ALSA;

}

int config_read(){
    config_struct_init();


    return 0;
}

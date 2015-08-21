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
#ifndef PAPAYA_GLOBALS_H
#define PAPAYA_GLOBALS_H

#define PAPAYA_MAX_CONCAT_FILENAME 2048

/* stupid colors */
#define PAPAYA_BLACK "\e[0;30m"
#define PAPAYA_BLUE "\e[0;34m"
#define PAPAYA_BLUEL "\e[1;34m"
#define PAPAYA_GREEN "\e[0;32m"
#define PAPAYA_GREENL "\e[1;32m"
#define PAPAYA_RED "\e[0;31m"
#define PAPAYA_REDL "\e[1;31m"
#define PAPAYA_CYAN "\e[0;36m"
#define PAPAYA_CYANL "\e[1;36m"
#define PAPAYA_VIOLET "\e[0;35m"
#define PAPAYA_VIOLETL "\e[1;35m"
#define PAPAYA_BROWN "\e[0;33m"
#define PAPAYA_YELLOW "\e[1;33m"
#define PAPAYA_GREY "\e[1;30m"
#define PAPAYA_GREYL "\e[0;37m"
#define PAPAYA_WHITE "\e[1;37m"
#define PAPAYA_QCOLOR "\e[00m"

struct ucmd {
    char *name;
    void *func;
    char *desc;
};

struct typed_command {
    char **cmd_argv;
    short int cmd_argc;
};

/* audio encoding format */
enum audio_format { 
    MP3, 
    OGG,
    AAC
};

enum video_format { 
    H264, 
    MPEG4
};

/* are we using a bitrate or a quality algorithm? */
enum encoder_type {
    QUALITY,
    BITRATE,
    QUANTIZER
};

enum plist_mode {
    SEQUENTIAL,
    SHUFFLE
};

enum a_mode {
    ALSA,
    PULSE,
    JACK
};

enum serv_mode {
    ICECAST_SERV_MODE,
    RTP_SERV_MODE
};


struct p_ice_config {
    char *hostname;
    short int port;
    enum audio_format aformat;
    char *username;
    char *password;
    char *mountpoint;
    char *stream_name;
    char *description;
    char *genre;
};

struct p_rtp_config {
    char *host;
    short int port;
    enum audio_format aformat;
    enum video_format vformat;
};

struct p_mp3_params {
    char *type;
    short int bitrate;
    short int quality;
};

struct p_vorbis_params {
    enum encoder_type type;
    short int bitrate;
    float quality;
};

struct p_h264_params {
    enum encoder_type type;
    int bitrate;
    short int quality;
};

struct p_alsa_params {
    gchar *device;
    gint64 buf_time;
    gint64 lat_time;
};

struct p_pulse_params {
};

struct p_live_audio {
    struct p_alsa_params *alsa_par;
    struct p_pulse_params *pulse_par;
};

struct p_live_video {
    gchar *device;
};

struct p_audio_settings {
    guint samplerate;
    guint channels;
};

struct p_video_settings {
    gchar *framerate;
    unsigned short int width;
    unsigned short int height;
};

struct x_grab_settings {
    unsigned short int startx;
    unsigned short int starty;
    unsigned short int endx;
    unsigned short int endy;
};

struct papaya_global_config {
    /* icecast configuration */
    struct p_ice_config *ice_server;
    /* rtp server configuration */
    struct p_rtp_config *rtp_server;

    /* lame and vorbis encoder configuration */
    struct p_mp3_params *mp3_par;
    struct p_vorbis_params *vorbis_par;

    struct p_h264_params *h264_par;

    /* this is the codec used for encoding */
    enum audio_format acodec;
    enum video_format vcodec;

    /* live audio configuration */
    struct p_live_audio *live_audio_par;
    struct p_live_video *live_video_par;

    struct p_audio_settings *audiosettings;
    struct p_video_settings *videosettings;
    struct x_grab_settings *xgrabsettings;


    enum plist_mode playlist_mode;
    enum a_mode audio_mode;
    enum serv_mode serving_mode;
};

typedef struct papaya_global_config globconf_t;

#endif

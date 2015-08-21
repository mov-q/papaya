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
#include <shell.h>

static void papaya_cmd_help(void);
/* readline func's */
char *command_generator(const char *, int);
char **papaya_completion(const char *, int, int);


/* It duplicates a string (the new string is malloc'd) */
char *
_dupstr(char *s) 
{

    char *r;

    r = malloc(strlen (s) + 1);
    strcpy(r, s);
    return(r);
}

/* the cmd table */
static void 
init_command_table()
{
    /* do nothing for now */
    rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";
    rl_filename_quote_characters = " \t\n\\\"'@<>=;|&()#$`?*[!:{~";
}

static void 
papaya_cmd_print_playing()
{
    char *ftrack = NULL;
    char *current_track_name = NULL; 

    current_track_name = get_current_track_name();
    if(current_track_name != NULL){
        ftrack = strrchr(get_current_track_name(),'/');
        ftrack++;
    }
    if(ftrack != NULL)
        g_print("%sWe're broadcasting:%s\n \t%s%s%s\n",
                PAPAYA_REDL,
                PAPAYA_QCOLOR,
                PAPAYA_GREY,
                ftrack,
                PAPAYA_QCOLOR);
}

static void 
papaya_cmd_print_sources()
{

    g_list_foreach(globsrc->sources_l, (GFunc) _print_sources, NULL);
}

static void 
papaya_cmd_print_outputs()
{

    g_list_foreach(glob_outputs->out_list, (GFunc) _print_outputs, NULL);
}

static void 
papaya_cmd_add_sine_source(void *dp)
{
    _cmd_add_sine_source();
}

static void 
papaya_cmd_remove_source(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    _cmd_remove_source(atoi(tc->cmd_argv[1]));
    g_print("%sSource removed!%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
}

static void 
papaya_cmd_remove_output(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    _cmd_remove_output(atoi(tc->cmd_argv[1]));
    g_print("%sOutput removed!%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
}

static void 
papaya_cmd_mute(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    _cmd_mute(atoi(tc->cmd_argv[1]));
    g_print("%sMuting source%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
}


static void 
papaya_cmd_pass_on(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    _cmd_pass_on();
    g_print("%sAudio out on!%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
}

static void 
papaya_cmd_unmute(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    _cmd_unmute(atoi(tc->cmd_argv[1]));
    g_print("%sUnmuting source%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
}


static void 
papaya_cmd_volup(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    if(_cmd_volup(atoi(tc->cmd_argv[1])))
        g_print("%sVolume up%s\n",
                PAPAYA_REDL,
                PAPAYA_QCOLOR);
}


static void 
papaya_cmd_voldown(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;

    if(_cmd_voldown(atoi(tc->cmd_argv[1])))
        g_print("%sVolume down%s\n",
                PAPAYA_REDL,
                PAPAYA_QCOLOR);
}

static void 
papaya_cmd_add_net(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;
    _cmd_add_net(tc->cmd_argv[1]);

}

static void 
papaya_cmd_add_audio_file(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;
    g_printf("comando %s", tc->cmd_argv[1]);
    _cmd_add_audio_file(tc);
}

static void 
papaya_cmd_add_box(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;
    _cmd_add_box();
}

static void 
papaya_cmd_add_alsa(void *dp)
{
    struct typed_command *tc;

    tc = (struct typed_command *) dp;
    _cmd_add_audio_live(tc->cmd_argv[1]);
}

static void 
papaya_cmd_quit()
{
    g_print("%sExiting...%s\n",
            PAPAYA_REDL,
            PAPAYA_QCOLOR);
    exit(0);
}




static struct ucmd cmd_vec[] = {
    {"print_sources", papaya_cmd_print_sources, "Print the active input sources"},
    {"print_outputs", papaya_cmd_print_outputs, "Print the active outputs"},
    {"print_current", papaya_cmd_print_playing, "Print the name of the playing track in a playlist"},
    {"add_sine", papaya_cmd_add_sine_source, "Add a sinusoidal test sound"},
    {"remove_source", papaya_cmd_remove_source, "arg: <position_id> Remove source by position"},
    {"remove_output", papaya_cmd_remove_output, "arg: <position_id> Remove output by position"},
    {"add_net", papaya_cmd_add_net, "arg: <url> Add an icecast/network stream as source"},
    {"add_alsa", papaya_cmd_add_alsa, "Add an alsa live input"},
    {"add_box", papaya_cmd_add_box, "->* add_box *<- Add a videobox"},
    {"add_audio_file", papaya_cmd_add_audio_file, "arg: <filename> ***WARNING: BUGGY **** Adds an audio file"},
    {"mute", papaya_cmd_mute, "arg: <position_id> Mute the source pointed by position_id"},
    {"pass_on", papaya_cmd_pass_on, "***WARNING: BUGGY*** activate an audio monitor"},
    {"unmute", papaya_cmd_unmute, "arg: <position_id> Unmute the source pointed by position_id"},
    {"volup", papaya_cmd_volup, "arg: <position_id> Volume up by one"},
    {"voldown", papaya_cmd_voldown, "arg: <position_id> Volume down by one"},
    {"help", papaya_cmd_help, "Print this help"},
    {"quit", papaya_cmd_quit, "Exit the program"},
    {(char *)NULL, (void *)NULL, (char *)NULL}
};

/* Initialize the readline and add completition to the shell */
void
readline_init(void) 
{

    rl_readline_name = "papaya";  
    rl_attempted_completion_function = papaya_completion;
}

char **
papaya_completion(const char *text, int start, int end) 
{

    char **matches;
    matches = (char **)NULL;

    if(start == 0)
        matches = rl_completion_matches(text, command_generator);

    return(matches);
}

char *
command_generator(const char *text, int state) 
{

    static int list_index, len;
    char *name;

    if(!state) {
        list_index = 0;
        len = strlen(text);
    }

    while(name = cmd_vec[list_index].name) {
        list_index++;

        if(strncmp(name, text, len) == 0)
            /* FIXME: memory leak, maybe, i've to check! */
            return(_dupstr(name));
    }

    return ((char *)NULL);
}


static void 
papaya_cmd_help()
{
    int idx;

    for(idx = 0; cmd_vec[idx].name; idx++){
        g_print("%s-*>%s%s %s %s%s<*-%s %s\n",
                PAPAYA_BLUEL,
                PAPAYA_QCOLOR,
                PAPAYA_GREENL,
                cmd_vec[idx].name,
                PAPAYA_QCOLOR,
                PAPAYA_BLUEL,
                PAPAYA_QCOLOR,
                cmd_vec[idx].desc);
    }
}

void 
(*get_cmd_ptr(const char *icmd))()
{
    int idx;
    void (*execfunc)(void *p_ptr);

    for(idx = 0; cmd_vec[idx].name; idx++){
        if( 0 == strcmp(cmd_vec[idx].name, icmd)){
            execfunc = cmd_vec[idx].func;
#ifdef _DEBUG_
            g_print("cmd_found: %s\n", cmd_vec[idx].name);
#endif
            return execfunc;
        }
    }

    return NULL;
}

char *
strtrim (char *s_str)
{
    unsigned len = strlen (s_str);
    unsigned char *str = (unsigned char *)s_str;
    unsigned char *s;

    for (s = str + len - 1; s >= str && *s && isblank(*s); --s)
        ;
    *(s + 1) = '\0';
    len = s - str;
    for (s = str; *s && isblank(*s); ++s)
        ;
    if (s != str)
        memmove (str, s, len - (str - s));
    return s_str;
}


static struct typed_command* 
get_command(char *icmd)
{
    register char *s;
    struct typed_command *newcom;
    char *new_s;
    char *elapsed;
    int idx;
    int nw = 0;
    char *wstart;

    /* ten arguments is enough, possibly */
    newcom = (struct typed_command *)malloc(sizeof(struct typed_command));
    newcom->cmd_argv = (char **) malloc (sizeof(char *) * 10);
    for(idx = 0; idx < 10; idx++)
        newcom->cmd_argv[idx] = NULL;

    s = icmd;

    while( (*s != '\0')){
        idx = 0;
        wstart = s;
        while(isblank(*s)){
            s++;
            wstart = s;
        }
        if(*s != '\"'){
            while((*s != '\0') && (!isblank(*s)) ){
                s++;
                idx++;
            }
        }
        else{
            wstart = ++s;
            while((*s != '\0') && (*s != '\"') ){
                s++;
                idx++;
            }	
        }
        new_s = (char *) malloc (sizeof(char) * (idx + 1));
        strncpy(new_s, wstart, idx);
        new_s[idx] = '\0';
        newcom->cmd_argv[nw++] = new_s;
    }

    newcom->cmd_argc = nw;

    return newcom;
}

static void 
clear_last_cmd(struct typed_command *lc)
{
    short int i;
    short int start = lc->cmd_argc - 1;
    for(i = start; i >= 0 ; i--){
        free(lc->cmd_argv[i]);
    }

    free(lc);
}

/* get the shell */
void 
get_control_shell()
{
    char prompt[] = "\e[1;32mpapaya\e[00m \e[0;31m$\e[00m ";
    char **acmd = NULL;
    char *cmd = NULL;
    short int exitcond = 0;
    void (*shellfunc)(void *p_ptr);
    struct typed_command *tcmd;

    /* a simple list of commands/callback
     * cb are mostly defined in streamer.c
     */
    init_command_table();

    /* init readline with auto-completation */
    readline_init();

    signal(SIGINT, p_sigint);

    do{
        if(cmd){
            free(cmd);
        }	
        cmd = readline(prompt);

        if(cmd && *cmd){
            add_history(cmd);

            tcmd = get_command(cmd);
            shellfunc = get_cmd_ptr(tcmd->cmd_argv[0]);
            /* here argv processing */

            if(shellfunc)
                (*shellfunc)((void *) tcmd);	

            clear_last_cmd(tcmd);
            //if( 0 == strcmpi(cmd, "exit"))
            //	exitcond = 1;
        }
    }
    /* typing exit stops the shell */	
    while (!exitcond);
}

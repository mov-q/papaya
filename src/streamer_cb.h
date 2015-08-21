#ifndef __STREAMER_CB_H
#define __STREAMER_CB_H

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include "sources.h"
#include "streamglobals.h"

gboolean net_event(GstPad *pad, GstEvent *te, gpointer u_data);
void cb_change_track2(gpointer *mdata);
gboolean cb_eos_in_filesrc(GstPad *pad, GstEvent *te, gpointer u_data);
#endif

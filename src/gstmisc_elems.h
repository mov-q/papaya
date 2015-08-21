#ifndef __GSTMISC_ELEMS_H
#define __GSTMISC_ELEMS_H
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include "streamglobals.h"
#include "confreader.h"
#include "pconfig.h"

GstElement* get_file_source (const char *fname);
GstElement* get_dvb_source (void);
GstElement* get_dv1394_source (void);
#endif

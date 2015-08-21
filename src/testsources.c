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
#include "testsources.h"

static GstElement* 
get_silence_source();

/* used for testing purposes */
GstElement* get_sin_bin()
{
	GstElement *silbin;

	GstElement *atestsrc;
	GstElement *raw_trans;

	GstPad *sinpad;

	silbin = gst_bin_new(NULL);

	atestsrc = get_silence_source();
	gst_bin_add(GST_BIN(silbin), atestsrc);

	raw_trans = get_raw_audio_trans();
	gst_bin_add(GST_BIN(silbin), raw_trans);
	gst_element_link(atestsrc, raw_trans);

	sinpad = gst_ghost_pad_new("src",
			gst_element_get_static_pad(raw_trans, "src"));
	gst_element_add_pad(silbin, sinpad);

	return silbin;
}

/* a silence bin always run on an adderbin channel
 * this _should_ keep stream continuity while adding
 * removing sources
 */
GstElement* 
get_silence_bin()
{
	GstElement *silbin;

	GstElement *atestsrc;
	GstElement *raw_trans;

	GstPad *testpad;

	silbin = gst_bin_new(NULL);

	atestsrc = get_silence_source();
	gst_bin_add(GST_BIN(silbin), atestsrc);
	g_object_set(G_OBJECT(atestsrc), "wave", 4, NULL);

	raw_trans = get_raw_audio_trans();
	gst_bin_add(GST_BIN(silbin), raw_trans);
	gst_element_link(atestsrc, raw_trans);

	testpad = gst_ghost_pad_new("src",
			gst_element_get_static_pad(raw_trans, "src"));
	gst_element_add_pad(silbin, testpad);

	return silbin;
}

/* add_sine and remove_sine are used to add and remove
 * a test signal to the stream while it is running.
 * clearly it makes no big sense, it is just sample code
 */
void 
_cmd_add_sine_source()
{
	GstElement *sinw;
	/* create a new element and throw it into 
	 * the pipeline
	 */
	sinw = get_sin_bin();
	gst_bin_add(GST_BIN(pipeline), sinw);

	/* link it to the adder and then play it : )
	*/
	add_new_stream_source(sinw, "src", "sinusoidal source");
	gst_element_set_state(sinw, GST_STATE_PLAYING);	

}

/* silence source to stream on one of the adder
 * channels continously
 */
static GstElement* 
get_silence_source()
{
	GstElement *ssrc;

	ssrc = gst_element_factory_make("audiotestsrc", "silencesrc");

	return ssrc;
}

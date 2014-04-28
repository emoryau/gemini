/*
 * TagExtractor.cpp
 *
 *  Created on: Jul 28, 2013
 *      Author: emoryau
 */

#include "TagExtractor.hpp"
#include <glib.h>
#include <gst/gst.h>

TagExtractor::TagExtractor() {
	file_uri = NULL;
	artist = NULL;
	album = NULL;
	title = NULL;
	track_number = 0;
	disc_number = 0;
	track_gain = 0.0;
	album_gain = 0.0;

	return;
}

TagExtractor::~TagExtractor() {
	freeData();
}

void TagExtractor::freeData() {
	if( file_uri != NULL )
		g_free( file_uri );
	if( artist != NULL )
		g_free( artist );
	if( album != NULL )
		g_free( album );
	if( title != NULL )
		g_free( title );
	file_uri = NULL;
	artist = NULL;
	album = NULL;
	title = NULL;
	track_number = 0;
	disc_number = 0;
	track_gain = 0.0;
	album_gain = 0.0;
}

void TagExtractor::readTags( const gchar* filename ) {
	freeData();

	GstElement *pipe, *dec, *sink;
	GstMessage *msg;

	GError* conversion_error = NULL;
	file_uri = g_filename_to_uri( filename, NULL, &conversion_error );

	pipe = gst_pipeline_new( "pipeline" );

	dec = gst_element_factory_make( "uridecodebin", NULL );
	g_object_set( dec, "uri", file_uri, NULL );
	gst_bin_add( GST_BIN (pipe), dec );

	sink = gst_element_factory_make( "fakesink", NULL );
	gst_bin_add( GST_BIN (pipe), sink );

	g_signal_connect( dec, "pad-added", G_CALLBACK (TagExtractor::onNewPad), sink );

	gst_element_set_state( pipe, GST_STATE_PAUSED );

	g_print( "%s ", filename );

	while( TRUE) {
		GstTagList *tags = NULL;

		msg = gst_bus_timed_pop_filtered( GST_ELEMENT_BUS (pipe), GST_CLOCK_TIME_NONE,
				(GstMessageType) (GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_TAG | GST_MESSAGE_ERROR) );

		if( GST_MESSAGE_TYPE (msg) != GST_MESSAGE_TAG ) /* error or async_done */
			break;

		gst_message_parse_tag( msg, &tags );

		// Try to read all of the tags we care about
		gst_tag_list_get_string( tags, GST_TAG_ARTIST, &artist );
		gst_tag_list_get_string( tags, GST_TAG_ALBUM, &album );
		gst_tag_list_get_string( tags, GST_TAG_TITLE, &title );
		gst_tag_list_get_uint( tags, GST_TAG_TRACK_NUMBER, &track_number );
		gst_tag_list_get_uint( tags, GST_TAG_ALBUM_VOLUME_NUMBER, &disc_number );
		gst_tag_list_get_double( tags, GST_TAG_TRACK_GAIN, &track_gain );
		gst_tag_list_get_double( tags, GST_TAG_ALBUM_GAIN, &album_gain );


		// This is for debugging, in case some files use non-standard tag names
		//printTags( tags );

		gst_tag_list_unref( tags );

		gst_message_unref( msg );
	};

	if( GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR ) {
    GError *err = NULL;
    gchar *dbg_info = NULL;

    g_printerr( "==\t%s\n", filename );
    gst_message_parse_error (msg, &err, &dbg_info);
    g_printerr ("ERROR from element %s: %s\n",
        GST_OBJECT_NAME (msg->src), err->message);
    g_printerr ("\tDebugging info: %s\n", (dbg_info) ? dbg_info : "none");
    g_error_free (err);
    g_free (dbg_info);
	} else {
		// TODO: Create a subsystem that can record missing important data
		if( artist == NULL )
			g_print( "ERROR: No Artist " );
		if( album == NULL )
			g_print( "ERROR: No album " );
		if( title == NULL )
			g_print( "ERROR: No title " );
		if( track_number == 0 )
			g_print( "ERROR: No Track Number ");
		if( disc_number == 0 )
			disc_number = 1;
		if( track_gain == 0.0 )
			g_print( "ERROR: No Track Gain " );
		if( album_gain == 0.0 ) {
			// TODO: Verify that clamping album gain to track gain is what we want to do.
			//g_print( "No Album Gain " );
			album_gain = track_gain;
		}
		g_print( "\n" );
	}

	gst_message_unref( msg );
	gst_element_set_state( pipe, GST_STATE_NULL );
	gst_object_unref( pipe );
	if( conversion_error != NULL )
		g_error_free( conversion_error );
	return;
}

void TagExtractor::parseOneTag( const GstTagList * list, const gchar * tag, gpointer user_data ) {
	int i, num;

	num = gst_tag_list_get_tag_size( list, tag );
	for (i = 0; i < num; ++i) {
		const GValue *val;

		val = gst_tag_list_get_value_index( list, tag, i );
		if( g_strcmp0( tag, "artist" ) == 0 ) {
			g_print( " artist: %s", g_value_get_string( val ) );
		}

	}
}

void TagExtractor::printOneTag( const GstTagList * list, const gchar * tag, gpointer user_data ) {
	int i, num;

	num = gst_tag_list_get_tag_size( list, tag );
	for (i = 0; i < num; ++i) {
		const GValue *val;

		/* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
		 * we only use the GValue approach here because it is more generic */
		val = gst_tag_list_get_value_index( list, tag, i );
		if( G_VALUE_HOLDS_STRING (val)) {
			g_print( "\t%20s : %s\n", tag, g_value_get_string( val ) );
		} else if( G_VALUE_HOLDS_UINT (val)) {
			g_print( "\t%20s : %u\n", tag, g_value_get_uint( val ) );
		} else if( G_VALUE_HOLDS_DOUBLE (val)) {
			g_print( "\t%20s : %g\n", tag, g_value_get_double( val ) );
		} else if( G_VALUE_HOLDS_BOOLEAN (val)) {
			g_print( "\t%20s : %s\n", tag, (g_value_get_boolean( val )) ? "true" : "false" );
		} else if( GST_VALUE_HOLDS_BUFFER (val)) {
			GstBuffer *buf = gst_value_get_buffer (val);
			guint buffer_size = gst_buffer_get_size( buf );

			g_print( "\t%20s : buffer of size %u\n", tag, buffer_size );
		} else if( GST_VALUE_HOLDS_DATE_TIME (val)) {
			GstDateTime *dt = (GstDateTime*) g_value_get_boxed( val );
			gchar *dt_str = gst_date_time_to_iso8601_string( dt );

			g_print( "\t%20s : %s\n", tag, dt_str );
			g_free( dt_str );
		} else {
			g_print( "\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
		}
	}
}

void TagExtractor::onNewPad( GstElement * dec, GstPad * pad, GstElement * fakesink ) {
	GstPad *sinkpad;

	sinkpad = gst_element_get_static_pad( fakesink, "sink" );
	if( !gst_pad_is_linked( sinkpad ) ) {
		if( gst_pad_link( pad, sinkpad ) != GST_PAD_LINK_OK )
			g_error( "Failed to link pads!" );
	}
	gst_object_unref( sinkpad );
}

void TagExtractor::printTags( GstTagList* tags ) {
	gst_tag_list_foreach( tags, TagExtractor::printOneTag, NULL );
	g_print( "\n" );
}

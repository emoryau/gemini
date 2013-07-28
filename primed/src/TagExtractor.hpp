/*
 * TagExtractor.hpp
 *
 *  Created on: Jul 28, 2013
 *      Author: emoryau
 */


#include <gst/gst.h>

class TagExtractor
{
public:
	TagExtractor();
	~TagExtractor();
	
	void freeData();
	void readTags( const gchar* filename );

	void printTags( GstTagList* tags );
	
	const gchar* getArtist() { return artist; };
	const gchar* getAlbum() { return album; };
	const gchar* getTitle() { return title; };
	const guint getTrackNumber() { return track_number; };
	const guint getDiscNumber() { return disc_number; };
	const gdouble getTrackGain() { return track_gain; };
	const gdouble getAlbumGain() { return album_gain; };

private:
	static void onNewPad (GstElement * dec, GstPad * pad, GstElement * fakesink);
	static void printOneTag (const GstTagList * list, const gchar * tag, gpointer user_data);
	
	gchar* artist;
	gchar* album;
	gchar* title;
	guint track_number;
	guint disc_number;
	gdouble track_gain;
	gdouble album_gain;
};

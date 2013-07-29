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
	
	const gchar* getArtist() const { return artist; };
	const gchar* getAlbum() const { return album; };
	const gchar* getTitle() const { return title; };
	const guint getTrackNumber() const { return track_number; };
	const guint getDiscNumber() const { return disc_number; };
	const gdouble getTrackGain() const { return track_gain; };
	const gdouble getAlbumGain() const { return album_gain; };

private:
	static void onNewPad (GstElement * dec, GstPad * pad, GstElement * fakesink);
	static void printOneTag (const GstTagList * list, const gchar * tag, gpointer user_data);
	static void parseOneTag( const GstTagList * list, const gchar * tag, gpointer user_data );


	gchar* artist;
	gchar* album;
	gchar* title;
	guint track_number;
	guint disc_number;
	gdouble track_gain;
	gdouble album_gain;
};

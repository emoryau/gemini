/*
 * Track.cpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#include "Track.hpp"

Track::Track() {
	artist = NULL;
	album = NULL;
	replayGain = -100.0f;
	trackNumber = 0;
	discNumber = 1;
}

Track::Track( TagExtractor& te ) {
	filename.assign( te.getFile_uri() );
	artFilename.assign( "" ); // TODO: Get art somehow
	if( te.getTitle() ) {
		name.assign( te.getTitle() );
	}
	artist = NULL;
	album = NULL;
	replayGain = te.getTrackGain();
	trackNumber = te.getTrackNumber();
	discNumber = te.getDiscNumber();
}

Track::~Track() {
	// TODO Auto-generated destructor stub
}

bool Track::operator<( const Track &t ) const {
	return filename < t.filename;
}

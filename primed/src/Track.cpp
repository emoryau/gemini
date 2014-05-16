/*
 * Track.cpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#include "Track.hpp"

Track::Track() {
	id = -1;
	artist = NULL;
	album = NULL;
	replayGain = -100.0f;
	trackNumber = 0;
	discNumber = 0;
}

Track::Track( TagExtractor& te ) {
	filename.assign( te.getFile_uri() );
	artFilename.assign( "" ); // TODO: Get art somehow
	if( te.getTitle() ) {
		name.assign( te.getTitle() );
	}
	id = -1;
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

void Track::print() {
	g_print( "%35s '%-25s' - %d.%-2d %s\n", artist ? artist->name.c_str() : "<no artist>",
			album ? album->name.c_str() : "<no album>", discNumber, trackNumber, name.c_str() );
}

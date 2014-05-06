/*
 * Album.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#include "Album.hpp"

Album::Album( TagExtractor& te ) {
	if( te.getAlbum() ) {
		name.assign( te.getAlbum() );
	}
	replayGain = te.getAlbumGain();
	artist = NULL;
	id = -1;
}


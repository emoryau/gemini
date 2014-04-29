/*
 * Album.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#include "Album.hpp"

Album::Album(TagExtractor& te) {
	name.assign(te.getAlbum());
	replayGain = te.getAlbumGain();
	artist = NULL;
}


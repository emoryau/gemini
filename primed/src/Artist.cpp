/*
 * Artist.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#include "Artist.hpp"

Artist::Artist(TagExtractor& te) {
	id = -1;
	if( te.getArtist() ) {
		name.assign(te.getArtist());
	}
}


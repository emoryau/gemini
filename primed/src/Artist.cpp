/*
 * Artist.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#include "Artist.hpp"

Artist::Artist(TagExtractor& te) {
	name.assign(te.getArtist());
}


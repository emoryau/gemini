/*
 * Album.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#ifndef ALBUM_HPP_
#define ALBUM_HPP_

#include <string>
#include "Artist.hpp"
#include "TagExtractor.hpp"

class Album {
public:
	Album() :
			id( -1 ), replayGain( -99.0 ), artist( NULL ) {
	};
	Album( TagExtractor& te );

	bool operator<( const Album &a ) const {
		return name < a.name;
	};

	long id;
	std::string name;
	float replayGain;
	Artist* artist;
};

#endif /* ALBUM_HPP_ */

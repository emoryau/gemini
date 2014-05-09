/*
 * Artist.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#ifndef ARTIST_HPP_
#define ARTIST_HPP_

#include <string>
#include "TagExtractor.hpp"

class Artist {
public:
	Artist(): id(-1) {};
	Artist( TagExtractor& te );

	bool operator<( const Artist &a ) const {
		return name < a.name;
	}
	;

	long id;
	std::string name;
};

#endif /* ARTIST_HPP_ */

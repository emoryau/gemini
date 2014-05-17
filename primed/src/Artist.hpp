/*
 * Artist.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#ifndef ARTIST_HPP_
#define ARTIST_HPP_

#include <string>

class Artist {
public:
	Artist(): id(-1), name("") {};

	bool operator<( const Artist &a ) const {
		return name < a.name;
	}
	;

	long id;
	std::string name;
};

#endif /* ARTIST_HPP_ */

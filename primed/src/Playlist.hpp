/*
 * Playlist.hpp
 *
 *  Created on: May 12, 2014
 *      Author: emory.au
 */

#ifndef PLAYLIST_HPP_
#define PLAYLIST_HPP_

#include <string>
#include <vector>

class Playlist {
public:
	typedef std::vector<long>::iterator TrackIdsIterator;

	long id;
	std::string name;
	std::vector<long> trackIds;
};

#endif /* PLAYLIST_HPP_ */

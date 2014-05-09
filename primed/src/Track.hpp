/*
 * Track.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include "TagExtractor.hpp"
#include "Artist.hpp"
#include "Album.hpp"

class Track {
public:
	Track();
	Track(TagExtractor& te);
	virtual ~Track();

	bool operator< (const Track &t) const;

	long id;
	std::string filename;
	std::string artFilename;
	std::string name;
	unsigned int trackNumber;
	unsigned int discNumber;
	double replayGain;
	Artist* artist;
	Album* album;
};

#endif /* TRACK_HPP_ */

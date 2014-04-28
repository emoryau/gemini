/*
 * Track.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>

class Track {
public:
	Track();
	virtual ~Track();

	bool operator< (const Track &t) const;

	std::string filename;
	std::string artFilename;
	std::string name;
	float replayGain;
	void* artist;
	void* album;
};

#endif /* TRACK_HPP_ */

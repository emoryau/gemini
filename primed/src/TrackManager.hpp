/*
 * TrackManager.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#ifndef TRACKMANAGER_HPP_
#define TRACKMANAGER_HPP_

#include <set>
#include <string>

#include "Track.hpp"
#include "TagExtractor.hpp"

class TrackManager {
public:
	TrackManager();
	virtual ~TrackManager();

	void Add(TagExtractor &te);

private:
	std::set<Track*> tracks;
};

#endif /* TRACKMANAGER_HPP_ */

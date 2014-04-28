/*
 * TrackManager.cpp
 *
 *  Created on: Apr 27, 2014
 *      Author: emoryau
 */

#include "TrackManager.hpp"

TrackManager::TrackManager() {
	// TODO Auto-generated constructor stub

}

TrackManager::~TrackManager() {
	// TODO Auto-generated destructor stub
}

void TrackManager::Add(TagExtractor &te) {
	Track* track = new Track();

	track->filename.assign(te.getFile_uri());
	track->name.assign(te.getTitle());

	tracks.insert(track);
}

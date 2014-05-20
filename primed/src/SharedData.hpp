/*
 * SharedData.hpp
 *
 *  Created on: May 20, 2014
 *      Author: emory.au
 */

#ifndef SHAREDDATA_HPP_
#define SHAREDDATA_HPP_

#include <boost/interprocess/sync/interprocess_semaphore.hpp>

struct SharedMetadata {
	SharedMetadata():
		version(1),
		semaphore(1),
		track_number(0),
		disc_number(0)
	{ };
	const char version;
	boost::interprocess::interprocess_semaphore semaphore;
	char name[256];
	char album[256];
	char artist[256];
	char album_artist[256];
	int track_number;
	int disc_number;
};

struct SharedPlaybackData {
	SharedPlaybackData():
		version(1),
		playback_seconds(0)
	{};
	const char version;
	long playback_seconds;
};

struct SharedLiveSearch {
	SharedLiveSearch():
		version(1),
		semaphore(1)
		{};
	char version;
	boost::interprocess::interprocess_semaphore semaphore;
	char live_search_query[256];
	char live_search_results[64][256];
};

struct SharedCommandQueue {
	SharedCommandQueue():
		version(1)
	{};
	const char version;
	char command_queue[64];
};

struct SharedData {
	SharedData() :
		version(1)
	{};
	const char version;
	SharedMetadata metadata;
	SharedPlaybackData playback_data;
	SharedCommandQueue command_queue;
	SharedLiveSearch live_search;
	char album_art[3*320*320];
};

#endif /* SHAREDDATA_HPP_ */

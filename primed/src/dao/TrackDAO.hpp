/*
 * TrackDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef TRACKDAO_HPP_
#define TRACKDAO_HPP_

#include "../Track.hpp"
#include <vector>


class TrackDAO {
public:
	virtual ~TrackDAO() = 0;
	virtual void ensureDBSchema() = 0;

	virtual void free( Track* track ) = 0;
	virtual void free( std::vector<long>* track_ids ) = 0;
	virtual Track* getTrack( Track* criterion ) = 0;
	virtual std::vector<long>* getTrackIds( ) = 0;
	virtual std::vector<long>* getTrackIdsByArtistId( long artist_id ) = 0;
	virtual std::vector<long>* getTrackIdsByAlbumId( long album_id ) = 0;
	virtual void insertOrUpdateTrack( Track* track ) = 0;
};

inline TrackDAO::~TrackDAO() { };

#endif /* TRACKDAO_HPP_ */

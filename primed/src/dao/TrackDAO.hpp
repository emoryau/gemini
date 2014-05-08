/*
 * TrackDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef TRACKDAO_HPP_
#define TRACKDAO_HPP_

#include "../Track.hpp"

class TrackDAO {
public:
	virtual ~TrackDAO() = 0;
	virtual void ensureDBSchema() = 0;

	virtual void free( Track* track ) = 0;
	virtual Track* getTrackById( long id ) = 0;
	virtual void insertOrUpdateTrack( Track* track ) = 0;
};

inline TrackDAO::~TrackDAO() { };

#endif /* TRACKDAO_HPP_ */

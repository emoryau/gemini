/*
 * TrackDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef TRACKDAO_HPP_
#define TRACKDAO_HPP_

#include "../../Track.hpp"

class TrackDAO {
public:
	virtual ~TrackDAO() = 0;

	virtual Track* GetTrackById( long id ) = 0;
	virtual void InsertOrUpdateTrack( Track* track ) = 0;
};


#endif /* TRACKDAO_HPP_ */

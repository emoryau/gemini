/*
 * DAOService.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef DAOFACTORY_HPP_
#define DAOFACTORY_HPP_

#include "TrackDAO.hpp"
#include "ArtistDAO.hpp"
#include "AlbumDAO.hpp"


class DAOFactory {
public:
	virtual ~DAOFactory() = 0;

	virtual void SetDBFile( const char* filename ) = 0;
	virtual TrackDAO* GetTrackDAO() = 0;
	virtual AlbumDAO* GetArtistDAO() = 0;
	virtual AlbumDAO* GetAlbumDAO() = 0;

};

inline DAOFactory::~DAOFactory() { };

#endif /* DAOFACTORY_HPP_ */

/*
 * DAOService.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef DAOFACTORY_HPP_
#define DAOFACTORY_HPP_

#include <string>
#include "TrackDAO.hpp"
#include "ArtistDAO.hpp"
#include "AlbumDAO.hpp"
#include "PlaylistDAO.hpp"
#include "SettingsDAO.hpp"

class DAOFactory {
public:
	static DAOFactory* CreateDAOFactoryFromURI( const char* db_uri );

	virtual ~DAOFactory() = 0;

	virtual void setDBFile( const char* filename ) = 0;
	virtual TrackDAO* getTrackDAO() = 0;
	virtual ArtistDAO* getArtistDAO() = 0;
	virtual AlbumDAO* getAlbumDAO() = 0;
	virtual PlaylistDAO* getPlaylistDAO() = 0;
	virtual SettingsDAO* getSettingsDAO() = 0;
};

inline DAOFactory::~DAOFactory() { };

#endif /* DAOFACTORY_HPP_ */

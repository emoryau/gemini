/*
 * DAOServiceSqlite3Impl.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef DAOFACTORYSQLITE3IMPL_HPP_
#define DAOFACTORYSQLITE3IMPL_HPP_

#include "dao/DAOFactory.hpp"
#include "dao/TrackDAO.hpp"
#include "dao/ArtistDAO.hpp"
#include "dao/AlbumDAO.hpp"
#include <sqlite3.h>

class DAOFactorySqlite3Impl: public DAOFactory {
public:
	DAOFactorySqlite3Impl();
	virtual ~DAOFactorySqlite3Impl();

	void setDBFile( const char* filename );
	virtual TrackDAO* getTrackDAO() { return trackDAO; };
	virtual ArtistDAO* getArtistDAO() { return artistDAO; };
	virtual AlbumDAO* getAlbumDAO() { return albumDAO; };
	virtual PlaylistDAO* getPlaylistDAO() { return playlistDAO; };

private:
	sqlite3* db;
	TrackDAO* trackDAO;
	ArtistDAO* artistDAO;
	AlbumDAO* albumDAO;
	PlaylistDAO* playlistDAO;
};

#endif /* DAOFACTORYSQLITE3IMPL_HPP_ */

/*
 * DAOServiceSqlite3Impl.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef DAOFACTORYSQLITE3IMPL_HPP_
#define DAOFACTORYSQLITE3IMPL_HPP_

#include "../DAOFactory.hpp"
#include "../TrackDAO.hpp"
#include "../ArtistDAO.hpp"
#include "../AlbumDAO.hpp"
#include <sqlite3.h>

class DAOFactorySqlite3Impl: public DAOFactory {
public:
	DAOFactorySqlite3Impl();
	virtual ~DAOFactorySqlite3Impl();

	void setDBFile( const char* filename );
	TrackDAO* getTrackDAO() { return trackDAO; };
	ArtistDAO* getArtistDAO() { return artistDAO; };
	AlbumDAO* getAlbumDAO() { return albumDAO; };

private:
	sqlite3* db;
	TrackDAO* trackDAO;
	ArtistDAO* artistDAO;
	AlbumDAO* albumDAO;
};

#endif /* DAOFACTORYSQLITE3IMPL_HPP_ */

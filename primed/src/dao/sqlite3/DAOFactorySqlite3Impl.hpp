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
#include <sqlite3.h>

class DAOFactorySqlite3Impl: public DAOFactory {
public:
	DAOFactorySqlite3Impl();
	virtual ~DAOFactorySqlite3Impl();

	void SetDBFile( const char* filename );
	TrackDAO* GetTrackDAO() { return trackDAO; };
	AlbumDAO* GetArtistDAO() { return NULL; };
	AlbumDAO* GetAlbumDAO() { return NULL; };

private:
	sqlite3* db;
	TrackDAO* trackDAO;
	ArtistDAO* artistDAO;
	AlbumDAO* albumDAO;
};

#endif /* DAOFACTORYSQLITE3IMPL_HPP_ */

/*
 * DAOServiceSqlite3Impl.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef DAOFACTORYSQLITE3IMPL_HPP_
#define DAOFACTORYSQLITE3IMPL_HPP_

#include "../DAOFactory.hpp"
#include <sqlite3.h>

class DAOFactorySqlite3Impl: public DAOFactory {
public:
	DAOFactorySqlite3Impl();
	virtual ~DAOFactorySqlite3Impl();

	void SetDBFile( const char* filename );
	TrackDAO* GetTrackDAO();
	AlbumDAO* GetArtistDAO();
	AlbumDAO* GetAlbumDAO();

};

#endif /* DAOFACTORYSQLITE3IMPL_HPP_ */

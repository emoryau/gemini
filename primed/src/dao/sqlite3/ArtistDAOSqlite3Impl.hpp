/*
 * ArtistDAOSqlite3Impl.hpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#ifndef ARTISTDAOSQLITE3IMPL_HPP_
#define ARTISTDAOSQLITE3IMPL_HPP_

#include "BaseSqlite3Impl.hpp"
#include <sqlite3.h>

class ArtistDAOSqlite3Impl: public ArtistDAO, public BaseSqlite3Impl {
public:
	ArtistDAOSqlite3Impl( sqlite3* db );
	virtual ~ArtistDAOSqlite3Impl();
	virtual void ensureDBSchema();

	virtual void free( Artist* track );
	virtual Artist* getArtistById( long id );
	virtual void insertOrUpdateArtist( Artist* track );
};

#endif /* ARTISTDAOSQLITE3IMPL_HPP_ */

/*
 * AlbumDAOSqlite3Impl.hpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#ifndef ALBUMDAOSQLITE3IMPL_HPP_
#define ALBUMDAOSQLITE3IMPL_HPP_

#include "BaseSqlite3Impl.hpp"
#include "../ArtistDAO.hpp"
#include "../AlbumDAO.hpp"

class AlbumDAOSqlite3Impl: public AlbumDAO, public BaseSqlite3Impl {
public:
	AlbumDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO);
	virtual ~AlbumDAOSqlite3Impl();
	virtual void ensureDBSchema();

	virtual void free( Album* album );
	virtual Album* getAlbumById( long id );
	virtual void insertOrUpdateAlbum( Album* album );

private:
	ArtistDAO* artistDAO;
};

#endif /* ALBUMDAOSQLITE3IMPL_HPP_ */

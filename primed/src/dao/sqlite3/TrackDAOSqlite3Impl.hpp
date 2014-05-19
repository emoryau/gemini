/*
 * TrackDAOSqlite3Impl.hpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#ifndef TRACKDAOSQLITE3IMPL_HPP_
#define TRACKDAOSQLITE3IMPL_HPP_

#include "../TrackDAO.hpp"
#include "../ArtistDAO.hpp"
#include "../AlbumDAO.hpp"
#include "BaseSqlite3Impl.hpp"
#include <sqlite3.h>

class TrackDAOSqlite3Impl: public TrackDAO, public BaseSqlite3Impl {
public:
	TrackDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO, AlbumDAO* albumDAO );
	virtual ~TrackDAOSqlite3Impl();

	virtual void free( Track* track );
	virtual void free( std::vector<long>* track_ids );
	virtual Track* getTrack( Track* criterion );
	virtual std::vector<long>* getTrackIds( );
	virtual std::vector<long>* getTrackIdsByArtistId( long artist_id );
	virtual std::vector<long>* getTrackIdsByAlbumId( long album_id );
	virtual void insertOrUpdateTrack( Track* track );

	virtual void ensureDBSchema();

private:
	ArtistDAO* artistDAO;
	AlbumDAO* albumDAO;
};


#endif /* TRACKDAOSQLITE3IMPL_HPP_ */

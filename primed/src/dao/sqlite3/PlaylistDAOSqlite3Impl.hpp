/*
 * PlaylistDAOSqlite3Impl.hpp
 *
 *  Created on: May 13, 2014
 *      Author: emory.au
 */

#ifndef PLAYLISTDAOSQLITE3IMPL_HPP_
#define PLAYLISTDAOSQLITE3IMPL_HPP_

#include "BaseSqlite3Impl.hpp"
#include "dao/PlaylistDAO.hpp"

class PlaylistDAOSqlite3Impl: public BaseSqlite3Impl, public PlaylistDAO {
public:
	PlaylistDAOSqlite3Impl( sqlite3* db ): BaseSqlite3Impl( db ) { };
	virtual ~PlaylistDAOSqlite3Impl();
	virtual void ensureDBSchema();

	virtual void free( Playlist* playlist );
	virtual Playlist* getPlaylist( Playlist* criterion );
	virtual void insertOrUpdatePlaylist( Playlist* playlist );
	virtual void deletePlaylist( Playlist* playlist );

protected:
	void fillPlaylistTrackIds( Playlist* playlist );
};

#endif /* PLAYLISTDAOSQLITE3IMPL_HPP_ */

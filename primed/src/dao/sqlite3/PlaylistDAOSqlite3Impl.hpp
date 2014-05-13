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
	virtual ~PlaylistDAOSqlite3Impl();
	PlaylistDAOSqlite3Impl( sqlite3* db );
	virtual void ensureDBSchema();

	virtual void free( Playlist* playlist );
	virtual Playlist* getPlaylist( Playlist* criterion );
	virtual void insertOrUpdatePlaylist( Playlist* playlist );

protected:
	void fillPlaylistTrackIds( Playlist* playlist );
};

#endif /* PLAYLISTDAOSQLITE3IMPL_HPP_ */

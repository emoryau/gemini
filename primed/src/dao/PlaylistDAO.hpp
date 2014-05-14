/*
 * PlaylistDAO.hpp
 *
 *  Created on: May 13, 2014
 *      Author: emory.au
 */

#ifndef PLAYLISTDAO_HPP_
#define PLAYLISTDAO_HPP_

#include "Playlist.hpp"

class PlaylistDAO {
public:
	virtual ~PlaylistDAO();
	virtual void ensureDBSchema() = 0;

	virtual void free( Playlist* playlist ) = 0;
	virtual Playlist* getPlaylist( Playlist* criterion ) = 0;
	virtual void insertOrUpdatePlaylist( Playlist* playlist ) = 0;
	virtual void deletePlaylist( Playlist* playlist ) = 0;
};

inline PlaylistDAO::~PlaylistDAO() { };

#endif /* PLAYLISTDAO_HPP_ */

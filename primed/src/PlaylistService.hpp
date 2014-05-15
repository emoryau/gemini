/*
 * PlaylistService.hpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#ifndef PLAYLISTSERVICE_HPP_
#define PLAYLISTSERVICE_HPP_

#include "Playlist.hpp"
#include "dao/DAOFactory.hpp"
#include "Track.hpp"

class PlaylistService {
public:
	enum PlaybackMode {
		EVERYTHING,
		ALBUM_SHUFFLED,
		ALBUM_ORDERED,
		ARTIST,
		CUSTOM
	};

	PlaylistService( DAOFactory* daoFactory );
	virtual ~PlaylistService();

	long getCurrentTrackId();

	void cueNextTrack();
	void cuePreviousTrack();

	void cueArtistById( long artist_id );
	void cueAlbumShuffledById( long album_id );
	void cueAlbumOrderedById( long album_id );
	void cueCustomPlaylist( Playlist* playlist );
	void exitMode();

	void createNewEverythingPlaylist();
	void refreshEverythingPlaylist();

	PlaybackMode getPlaybackMode() { return mode; };

private:
	static const char* EVERYTHING_PLAYLIST_NAME;
	static const char* SPECIAL_PLAYLIST_NAME;

	DAOFactory* daoFactory;

	Playlist* everythingPlaylist;
	Playlist::TrackIdsIterator everythingPlaylistIter;

	Playlist* currentPlaylist;
	Playlist::TrackIdsIterator currentPlaylistIter;

	PlaybackMode mode;
};

#endif /* PLAYLISTSERVICE_HPP_ */

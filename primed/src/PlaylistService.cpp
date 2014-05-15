/*
 * PlaylistService.cpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#include <algorithm>
#include <glib.h>
#include "PlaylistService.hpp"
#include "GeminiException.hpp"

const char* PlaylistService::EVERYTHING_PLAYLIST_NAME = "everything";
const char* PlaylistService::SPECIAL_PLAYLIST_NAME = "special";

PlaylistService::PlaylistService( DAOFactory* daoFactory ): daoFactory(daoFactory), mode(EVERYTHING) {
	Playlist everythingCriterion;
	everythingCriterion.name.assign( EVERYTHING_PLAYLIST_NAME );
	everythingPlaylist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );

	if( everythingPlaylist == NULL ) {
		createNewEverythingPlaylist();
		everythingPlaylist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );
		everythingPlaylistIter = everythingPlaylist->trackIds.begin();
	} else {
		// TODO: restore everythingPlaylistIter from storage
		everythingPlaylistIter = everythingPlaylist->trackIds.begin();
	}

	// TODO: restore mode from storage

	if( mode == EVERYTHING ) {
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
	} else {
		Playlist specialCriterion;
		specialCriterion.name.assign( SPECIAL_PLAYLIST_NAME );
		currentPlaylist = daoFactory->getPlaylistDAO()->getPlaylist( &specialCriterion );
		// TODO: restore current playlist from storage
		currentPlaylistIter = currentPlaylist->trackIds.begin();
	}
}

PlaylistService::~PlaylistService() {
	exitMode();
	daoFactory->getPlaylistDAO()->free( everythingPlaylist );
}

long PlaylistService::getCurrentTrackId() {
	return *currentPlaylistIter;
}

void PlaylistService::cueNextTrack() {
	currentPlaylistIter++;
	if( mode == EVERYTHING ) {
		everythingPlaylistIter = currentPlaylistIter;
	}
	if( currentPlaylistIter == currentPlaylist->trackIds.end() ) {
		if( mode == EVERYTHING ) {
			currentPlaylistIter = currentPlaylist->trackIds.begin();
			everythingPlaylistIter = currentPlaylistIter;
		} else {
			exitMode();
			everythingPlaylistIter++;
			currentPlaylistIter = everythingPlaylistIter;
			currentPlaylist = everythingPlaylist;
			mode = EVERYTHING;
		}
	}
}

void PlaylistService::cuePreviousTrack() {
	if( currentPlaylistIter == currentPlaylist->trackIds.begin() ) {
		if( mode == EVERYTHING ) {
			return;
		} else {
			currentPlaylistIter = everythingPlaylistIter;
			daoFactory->getPlaylistDAO()->free( currentPlaylist );
			currentPlaylist = everythingPlaylist;
			mode = EVERYTHING;
		}
	}
	currentPlaylistIter--;
	if( mode == EVERYTHING ) {
		everythingPlaylistIter = currentPlaylistIter;
	}
}

void PlaylistService::cueArtistById( long artist_id ) {
	long current_track_id = *currentPlaylistIter;
	exitMode();
	currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByArtist( artist_id );
	if( currentPlaylist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		return;
	}
	std::random_shuffle( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end() );
	// Move current track to beginning
	Playlist::TrackIdsIterator shuffledIter = std::find( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end(), current_track_id );
	if( shuffledIter != currentPlaylist->trackIds.end() ) {
		std::swap( *currentPlaylist->trackIds.begin(), *shuffledIter );
	}
	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ARTIST;
}

void PlaylistService::cueAlbumShuffledById( long album_id ) {
	exitMode();
	currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( album_id );
	if( currentPlaylist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
		return;
	}
	std::random_shuffle( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end() );

	// Move current track to beginning
	Playlist::TrackIdsIterator shuffledIter = std::find( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end(), *everythingPlaylistIter );
	if( shuffledIter == currentPlaylist->trackIds.end() ) {
		// TODO: Report error in db: Could not find current track
	} else {
		std::swap( *currentPlaylist->trackIds.begin(), *shuffledIter );
	}

	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ALBUM_SHUFFLED;
}

void PlaylistService::cueAlbumOrderedById( long album_id ) {
	exitMode();
	currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( album_id );
	if( currentPlaylist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
		return;
	}
	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ALBUM_ORDERED;
}

void PlaylistService::cueCustomPlaylist( Playlist* playlist ) {
	if( playlist == NULL ) {
		//TODO: report error
		return;
	}
	exitMode();
	currentPlaylist = new Playlist( *playlist );
	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = CUSTOM;
}

void PlaylistService::createNewEverythingPlaylist() {
	Playlist* newEverythingPlaylist = daoFactory->getTrackDAO()->getTrackIds();
	newEverythingPlaylist->name.assign( EVERYTHING_PLAYLIST_NAME );
	std::random_shuffle( newEverythingPlaylist->trackIds.begin(), newEverythingPlaylist->trackIds.end() );
	daoFactory->getPlaylistDAO()->insertOrUpdatePlaylist( newEverythingPlaylist );
	daoFactory->getTrackDAO()->free( newEverythingPlaylist );
}

void PlaylistService::refreshEverythingPlaylist() {
	if( everythingPlaylist == NULL ) {
		THROW_GEMINI_EXCEPTION( "Trying to refresh with null everything playlist" );
	}
	THROW_GEMINI_EXCEPTION( "PlaylistService::refreshEverythingPlaylist() unimplemented" );
}

void PlaylistService::exitMode() {
	switch( mode ) {
		case EVERYTHING:
			break;
		case CUSTOM:
			if( currentPlaylist != NULL && currentPlaylist != everythingPlaylist ) {
				delete currentPlaylist;
				currentPlaylist = NULL;
			}
			break;
		default:
			if( currentPlaylist != NULL && currentPlaylist != everythingPlaylist ) {
				daoFactory->getPlaylistDAO()->free( currentPlaylist );
				currentPlaylist = NULL;
			}
	}
}

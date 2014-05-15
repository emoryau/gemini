/*
 * PlaylistService.cpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#include <algorithm>
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
	if( currentPlaylistIter == currentPlaylist->trackIds.end() ) {
		if( mode == EVERYTHING ) {
			currentPlaylistIter = currentPlaylist->trackIds.begin();
			everythingPlaylistIter = currentPlaylistIter;
		} else {
			exitMode();
			currentPlaylistIter = ++everythingPlaylistIter;
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
		}
	}
	currentPlaylistIter--;
	if( mode == EVERYTHING ) {
		everythingPlaylistIter = currentPlaylistIter;
	}
}

void PlaylistService::cueCurrentArtist() {
	Track criterion;
	criterion.id = *currentPlaylistIter;
	Track* track = daoFactory->getTrackDAO()->getTrack( &criterion );
	if( track->artist == NULL ) {
		// TODO: notify of error
		return;
	}
	exitMode();
	if( track->album->artist != NULL ) {
		currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByArtist( track->album->artist->id );
	} else {
		currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByArtist( track->artist->id );
	}
	std::random_shuffle( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end() );
	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ARTIST;
}

void PlaylistService::cueCurrentAlbumShuffled() {
	Track criterion;
	criterion.id = *currentPlaylistIter;
	Track* track = daoFactory->getTrackDAO()->getTrack( &criterion );
	if( track->album == NULL ) {
		// TODO: notify of error
		return;
	}
	exitMode();
	currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( track->album->id );
	std::random_shuffle( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end() );

	// Move current track to beginning
	Playlist::TrackIdsIterator shuffledIter = std::find( currentPlaylist->trackIds.begin(), currentPlaylist->trackIds.end(), track->id );
	std::swap( *currentPlaylist->trackIds.begin(), *shuffledIter );

	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ALBUM_SHUFFLED;
	daoFactory->getTrackDAO()->free( track );
}

void PlaylistService::cueCurrentAlbumOrdered() {
	Track criterion;
	criterion.id = *currentPlaylistIter;
	Track* track = daoFactory->getTrackDAO()->getTrack( &criterion );
	if( track->album == NULL ) {
		// TODO: notify of error
		return;
	}
	exitMode();
	currentPlaylist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( track->album->id );
	currentPlaylistIter = currentPlaylist->trackIds.begin();
	mode = ALBUM_ORDERED;
	daoFactory->getTrackDAO()->free( track );
}

void PlaylistService::cueCustomPlaylist( Playlist* playlist ) {
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

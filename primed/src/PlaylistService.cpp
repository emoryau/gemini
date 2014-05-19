/*
 * PlaylistService.cpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#include <algorithm>
#include <set>
#include <glib.h>
#include "PlaylistService.hpp"
#include "GeminiException.hpp"

const char* PlaylistService::EVERYTHING_PLAYLIST_NAME = "everything";
const char* PlaylistService::SPECIAL_PLAYLIST_NAME = "special";
const char* PlaylistService::SETTING_EVERYTHING_PLAYLIST_POSITION = "everything_playlist_position";
const char* PlaylistService::SETTING_CURRENT_PLAYLIST_POSITION = "current_playlist_position";
const char* PlaylistService::SETTING_PLAYLIST_SERVICE_MODE = "playlist_service_mode";

PlaylistService::PlaylistService( DAOFactory* daoFactory ): daoFactory(daoFactory), mode(EVERYTHING) {
	Playlist everythingCriterion;
	everythingCriterion.name.assign( EVERYTHING_PLAYLIST_NAME );
	everythingPlaylist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );

	if( everythingPlaylist == NULL ) {
		createNewEverythingPlaylist();
		everythingPlaylist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );
		everythingPlaylistIter = everythingPlaylist->trackIds.begin();
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
	} else {
		restoreState();
	}
}

PlaylistService::~PlaylistService() {
	saveState();
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
	Playlist* new_playlist = daoFactory->getTrackDAO()->getTrackIdsByArtist( artist_id );
	if( new_playlist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
		return;
	}
	currentPlaylist = new Playlist( *new_playlist );
	daoFactory->getTrackDAO()->free( new_playlist );
	currentPlaylist->name.assign( SPECIAL_PLAYLIST_NAME );

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
	Playlist* new_playlist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( album_id );
	if( new_playlist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
		return;
	}
	currentPlaylist = new Playlist( *new_playlist );
	daoFactory->getTrackDAO()->free( new_playlist );
	currentPlaylist->name.assign( SPECIAL_PLAYLIST_NAME );
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
	Playlist* new_playlist = daoFactory->getTrackDAO()->getTrackIdsByAlbum( album_id );
	if( new_playlist == NULL ) {
		//TODO: report error
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
		mode = EVERYTHING;
		return;
	}
	currentPlaylist = new Playlist( *new_playlist );
	daoFactory->getTrackDAO()->free( new_playlist );
	currentPlaylist->name.assign( SPECIAL_PLAYLIST_NAME );
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
	currentPlaylist->name.assign( SPECIAL_PLAYLIST_NAME );
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

void PlaylistService::saveState() {
	SettingsDAO* settingsDAO = daoFactory->getSettingsDAO();

	long everythingPlaylistPosition = everythingPlaylistIter - everythingPlaylist->trackIds.begin();
	long currentPlaylistPosition = currentPlaylistIter - currentPlaylist->trackIds.begin();
	long saveMode_l = mode;

	settingsDAO->set( SETTING_EVERYTHING_PLAYLIST_POSITION, everythingPlaylistPosition );
	settingsDAO->set( SETTING_CURRENT_PLAYLIST_POSITION, currentPlaylistPosition );
	settingsDAO->set( SETTING_PLAYLIST_SERVICE_MODE, saveMode_l );

	if( mode != EVERYTHING ) {
		currentPlaylist->name.assign( SPECIAL_PLAYLIST_NAME );
		daoFactory->getPlaylistDAO()->insertOrUpdatePlaylist( currentPlaylist );
	}
}

void PlaylistService::refreshEverythingPlaylist() {
	if( everythingPlaylist == NULL ) {
		THROW_GEMINI_EXCEPTION( "Trying to refresh with null everything playlist" );
	}

	Playlist* track_table = daoFactory->getTrackDAO()->getTrackIds();
	std::set<long> track_table_set( track_table->trackIds.begin(), track_table->trackIds.end() );
	std::set<long> everything_set( everythingPlaylist->trackIds.begin(), everythingPlaylist->trackIds.end() );
	daoFactory->getTrackDAO()->free( track_table );

	std::set<long> tracks_not_in_everything;
	std::set<long> tracks_not_in_table;

	for( std::set<long>::iterator table_iter = track_table_set.begin(); table_iter != track_table_set.end(); table_iter++ ) {
		if( everything_set.find( *table_iter ) == everything_set.end() ) {
			tracks_not_in_everything.insert( *table_iter );
		}
	}
	for( std::set<long>::iterator everything_iter = everything_set.begin(); everything_iter != everything_set.end(); everything_iter++ ) {
		if( track_table_set.find( *everything_iter ) == track_table_set.end() ) {
			tracks_not_in_table.insert( *everything_iter );
		}
	}
	g_message( "In Tracks table, but missing from everything playlist:" );
	for( std::set<long>::iterator missing_iter = tracks_not_in_everything.begin(); missing_iter != tracks_not_in_everything.end(); missing_iter++ ) {
		g_message("\t%d", *missing_iter);
	}
	g_message( "In Everything Playlist, but missing from tracks table:" );
	for( std::set<long>::iterator missing_iter = tracks_not_in_table.begin(); missing_iter != tracks_not_in_table.end(); missing_iter++ ) {
		g_message("\t%d", *missing_iter);
	}

	// Rebuild everything playlist trackid vector in the same order, but skip trackids which are no longer in the tracks table
	std::vector<long> new_everything_trackids;
	new_everything_trackids.resize( everythingPlaylist->trackIds.size() );
	for( Playlist::TrackIdsIterator everything_iter = everythingPlaylist->trackIds.begin(); everything_iter != everythingPlaylist->trackIds.end(); everything_iter++ ) {
		if( tracks_not_in_table.find( *everything_iter ) == tracks_not_in_table.end() ) {
			new_everything_trackids.push_back( *everything_iter );
		} else {
			// This track is being dropped from the playlist - advance the iterator if it points to this track
			if( (*everything_iter) == (*everythingPlaylistIter) ) {
				everythingPlaylistIter++;
			}
		}
	}
	long everything_playlist_iter_trackid = *everythingPlaylistIter;
	everythingPlaylist->trackIds.assign( new_everything_trackids.begin(), new_everything_trackids.end() );

	// Restore everythingPlaylistIter
	for( Playlist::TrackIdsIterator everything_search_iter = everythingPlaylist->trackIds.begin(); everything_search_iter != everythingPlaylist->trackIds.end(); everything_search_iter++ ) {
		if( *everything_search_iter == everything_playlist_iter_trackid ) {
			everythingPlaylistIter = everything_search_iter;
		}
	}

	// Add tracks from the track table which are missing from the everything playlist
	everythingPlaylist->trackIds.insert( everythingPlaylist->trackIds.end(), tracks_not_in_everything.begin(), tracks_not_in_everything.end() );
	std::random_shuffle( everythingPlaylistIter+1, everythingPlaylist->trackIds.end() );
	daoFactory->getPlaylistDAO()->insertOrUpdatePlaylist( everythingPlaylist );
}

void PlaylistService::exitMode() {
	switch( mode ) {
		case EVERYTHING:
			break;
		case ARTIST:
		case ALBUM_SHUFFLED:
		case ALBUM_ORDERED:
		case CUSTOM:
			if( currentPlaylist != NULL && currentPlaylist != everythingPlaylist ) {
				delete currentPlaylist;
			}
			break;
	}
	currentPlaylist = everythingPlaylist;
	currentPlaylistIter = everythingPlaylistIter;
	mode = EVERYTHING;
}

void PlaylistService::restoreState() {
	long everything_playlist_position;
	long current_playlist_position;
	long restore_mode_l;
	SettingsDAO* settingsDAO = daoFactory->getSettingsDAO();

	settingsDAO->get( SETTING_EVERYTHING_PLAYLIST_POSITION, everything_playlist_position );
	settingsDAO->get( SETTING_CURRENT_PLAYLIST_POSITION, current_playlist_position );
	settingsDAO->get( SETTING_PLAYLIST_SERVICE_MODE, restore_mode_l );

	if( everythingPlaylist->trackIds.end() - everythingPlaylist->trackIds.begin() <= everything_playlist_position ) {
		g_error( "Saved everythingPlaylistPosition is too large. Reverting to beginning" );
		everything_playlist_position = 0;
	}
	everythingPlaylistIter = everythingPlaylist->trackIds.begin() + everything_playlist_position;

	mode = (PlaybackMode) restore_mode_l;

	if( mode != EVERYTHING ) {
		Playlist criterion;
		criterion.name.assign( SPECIAL_PLAYLIST_NAME );
		Playlist* new_playlist = daoFactory->getPlaylistDAO()->getPlaylist( &criterion );
		if( new_playlist == NULL ) {
			// TODO: report error
			currentPlaylist = everythingPlaylist;
			currentPlaylistIter = everythingPlaylistIter;
			mode = EVERYTHING;
		} else {
			currentPlaylist = new Playlist( *new_playlist );
			daoFactory->getPlaylistDAO()->free( new_playlist );
		}

		if( currentPlaylist->trackIds.end() - currentPlaylist->trackIds.begin() <= current_playlist_position ) {
			g_error( "Saved current_playlist_position is too large. Reverting to beginning" );
			current_playlist_position = 0;
		}
		currentPlaylistIter = currentPlaylist->trackIds.begin() + current_playlist_position;
	} else {
		currentPlaylist = everythingPlaylist;
		currentPlaylistIter = everythingPlaylistIter;
	}
}

/*
 * PlaylistService.cpp
 *
 *  Created on: May 14, 2014
 *      Author: emoryau
 */

#include <algorithm>
#include <vector>
#include <set>
#include <cstdlib>
#include <glib.h>
#include "PlaylistService.hpp"
#include "GeminiException.hpp"

const char* PlaylistService::EVERYTHING_PLAYLIST_NAME = "everything";
const char* PlaylistService::SPECIAL_PLAYLIST_NAME = "special";
const char* PlaylistService::SETTING_EVERYTHING_PLAYLIST_POSITION = "everything_playlist_position";
const char* PlaylistService::SETTING_CURRENT_PLAYLIST_POSITION = "current_playlist_position";
const char* PlaylistService::SETTING_PLAYLIST_SERVICE_MODE = "playlist_service_mode";

PlaylistService::PlaylistService( DAOFactory* daoFactory ): dao_factory(daoFactory), mode(EVERYTHING) {
	Playlist everythingCriterion;
	everythingCriterion.name.assign( EVERYTHING_PLAYLIST_NAME );
	everything_playlist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );

	if( everything_playlist == NULL ) {
		createNewEverythingPlaylist();
		everything_playlist = daoFactory->getPlaylistDAO()->getPlaylist( &everythingCriterion );
		everything_playlist_iter = everything_playlist->track_ids.begin();
		current_playlist = everything_playlist;
		current_playlist_iter = everything_playlist_iter;
		mode = EVERYTHING;
	} else {
		restoreState();
	}
}

PlaylistService::~PlaylistService() {
	saveState();
	exitMode();
	dao_factory->getPlaylistDAO()->free( everything_playlist );
}

long PlaylistService::getCurrentTrackId() {
	return *current_playlist_iter;
}

void PlaylistService::cueNextTrack() {
	current_playlist_iter++;
	if( mode == EVERYTHING ) {
		everything_playlist_iter = current_playlist_iter;
	}
	if( current_playlist_iter == current_playlist->track_ids.end() ) {
		if( mode == EVERYTHING ) {
			current_playlist_iter = current_playlist->track_ids.begin();
			everything_playlist_iter = current_playlist_iter;
		} else {
			exitMode();
			everything_playlist_iter++;
			current_playlist_iter = everything_playlist_iter;
			current_playlist = everything_playlist;
			mode = EVERYTHING;
		}
	}
}

void PlaylistService::cuePreviousTrack() {
	if( current_playlist_iter == current_playlist->track_ids.begin() ) {
		if( mode == EVERYTHING ) {
			return;
		} else {
			current_playlist_iter = everything_playlist_iter;
			dao_factory->getPlaylistDAO()->free( current_playlist );
			current_playlist = everything_playlist;
			mode = EVERYTHING;
		}
	}
	current_playlist_iter--;
	if( mode == EVERYTHING ) {
		everything_playlist_iter = current_playlist_iter;
	}
}

void PlaylistService::cueArtistById( long artist_id ) {
	long current_track_id = *current_playlist_iter;
	exitMode();
	std::vector<long>* new_playlist_ids = dao_factory->getTrackDAO()->getTrackIdsByArtistId( artist_id );
	if( new_playlist_ids == NULL ) {
		//TODO: report error
		current_playlist = everything_playlist;
		current_playlist_iter = everything_playlist_iter;
		mode = EVERYTHING;
		return;
	}
	current_playlist = new Playlist();
	current_playlist->track_ids = *new_playlist_ids;
	current_playlist->name.assign( SPECIAL_PLAYLIST_NAME );
	dao_factory->getTrackDAO()->free( new_playlist_ids );

	std::random_shuffle( current_playlist->track_ids.begin(), current_playlist->track_ids.end(), playlistRandom );
	// Move current track to beginning
	Playlist::TrackIdsIterator shuffledIter = std::find( current_playlist->track_ids.begin(), current_playlist->track_ids.end(), current_track_id );
	if( shuffledIter != current_playlist->track_ids.end() ) {
		std::swap( *current_playlist->track_ids.begin(), *shuffledIter );
	}
	current_playlist_iter = current_playlist->track_ids.begin();
	mode = ARTIST;
}

void PlaylistService::cueAlbumShuffledById( long album_id ) {
	exitMode();
	std::vector<long>* new_playlist_ids = dao_factory->getTrackDAO()->getTrackIdsByAlbumId( album_id );
	if( new_playlist_ids == NULL ) {
		//TODO: report error
		current_playlist = everything_playlist;
		current_playlist_iter = everything_playlist_iter;
		mode = EVERYTHING;
		return;
	}
	current_playlist = new Playlist();
	current_playlist->name.assign( SPECIAL_PLAYLIST_NAME );
	current_playlist->track_ids = *new_playlist_ids;
	dao_factory->getTrackDAO()->free( new_playlist_ids );
	std::random_shuffle( current_playlist->track_ids.begin(), current_playlist->track_ids.end() );

	// Move current track to beginning
	Playlist::TrackIdsIterator shuffled_iter = std::find( current_playlist->track_ids.begin(), current_playlist->track_ids.end(), *everything_playlist_iter );
	if( shuffled_iter == current_playlist->track_ids.end() ) {
		// TODO: Report error in db: Could not find current track
	} else {
		std::swap( *current_playlist->track_ids.begin(), *shuffled_iter );
	}

	current_playlist_iter = current_playlist->track_ids.begin();
	mode = ALBUM_SHUFFLED;
}

void PlaylistService::cueAlbumOrderedById( long album_id ) {
	exitMode();
	std::vector<long>* new_playlist_ids = dao_factory->getTrackDAO()->getTrackIdsByAlbumId( album_id );
	if( new_playlist_ids == NULL ) {
		//TODO: report error
		current_playlist = everything_playlist;
		current_playlist_iter = everything_playlist_iter;
		mode = EVERYTHING;
		return;
	}
	current_playlist = new Playlist();
	current_playlist->track_ids = *new_playlist_ids;
	current_playlist->name.assign( SPECIAL_PLAYLIST_NAME );
	dao_factory->getTrackDAO()->free( new_playlist_ids );
	current_playlist_iter = current_playlist->track_ids.begin();
	mode = ALBUM_ORDERED;
}

void PlaylistService::cueCustomPlaylist( Playlist* playlist ) {
	if( playlist == NULL ) {
		//TODO: report error
		return;
	}
	exitMode();
	current_playlist = new Playlist( *playlist );
	current_playlist->name.assign( SPECIAL_PLAYLIST_NAME );
	current_playlist_iter = current_playlist->track_ids.begin();
	mode = CUSTOM;
}

void PlaylistService::createNewEverythingPlaylist() {
	std::vector<long>* new_everything_playlist_ids = dao_factory->getTrackDAO()->getTrackIds();
	Playlist* new_everything_playlist = new Playlist();
	new_everything_playlist->track_ids = *new_everything_playlist_ids;
	dao_factory->getTrackDAO()->free( new_everything_playlist_ids );
	new_everything_playlist->name.assign( EVERYTHING_PLAYLIST_NAME );
	std::random_shuffle( new_everything_playlist->track_ids.begin(), new_everything_playlist->track_ids.end() );
	dao_factory->getPlaylistDAO()->insertOrUpdatePlaylist( new_everything_playlist );
}

void PlaylistService::saveState() {
	SettingsDAO* settingsDAO = dao_factory->getSettingsDAO();

	long everythingPlaylistPosition = everything_playlist_iter - everything_playlist->track_ids.begin();
	long currentPlaylistPosition = current_playlist_iter - current_playlist->track_ids.begin();
	long saveMode_l = mode;

	settingsDAO->set( SETTING_EVERYTHING_PLAYLIST_POSITION, everythingPlaylistPosition );
	settingsDAO->set( SETTING_CURRENT_PLAYLIST_POSITION, currentPlaylistPosition );
	settingsDAO->set( SETTING_PLAYLIST_SERVICE_MODE, saveMode_l );

	if( mode != EVERYTHING ) {
		current_playlist->name.assign( SPECIAL_PLAYLIST_NAME );
		dao_factory->getPlaylistDAO()->insertOrUpdatePlaylist( current_playlist );
	}
}

void PlaylistService::refreshEverythingPlaylist() {
	if( everything_playlist == NULL ) {
		THROW_GEMINI_EXCEPTION( "Trying to refresh with null everything playlist" );
	}

	std::vector<long>* track_table_ids = dao_factory->getTrackDAO()->getTrackIds();
	std::set<long> track_table_set( track_table_ids->begin(), track_table_ids->end() );
	std::set<long> everything_set( everything_playlist->track_ids.begin(), everything_playlist->track_ids.end() );
	dao_factory->getTrackDAO()->free( track_table_ids );

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
	new_everything_trackids.reserve( everything_playlist->track_ids.size() );
	for( Playlist::TrackIdsIterator everything_iter = everything_playlist->track_ids.begin(); everything_iter != everything_playlist->track_ids.end(); everything_iter++ ) {
		if( tracks_not_in_table.find( *everything_iter ) == tracks_not_in_table.end() ) {
			new_everything_trackids.push_back( *everything_iter );
		} else {
			// This track is being dropped from the playlist - advance the iterator if it points to this track
			if( (*everything_iter) == (*everything_playlist_iter) ) {
				everything_playlist_iter++;
			}
		}
	}
	long everything_playlist_iter_trackid = *everything_playlist_iter;
	everything_playlist->track_ids.assign( new_everything_trackids.begin(), new_everything_trackids.end() );

	// Restore everythingPlaylistIter
	for( Playlist::TrackIdsIterator everything_search_iter = everything_playlist->track_ids.begin(); everything_search_iter != everything_playlist->track_ids.end(); everything_search_iter++ ) {
		if( *everything_search_iter == everything_playlist_iter_trackid ) {
			everything_playlist_iter = everything_search_iter;
			if( mode == EVERYTHING ) {
				current_playlist_iter = everything_playlist_iter;
			}
		}
	}

	// Add tracks from the track table which are missing from the everything playlist
	everything_playlist->track_ids.insert( everything_playlist->track_ids.end(), tracks_not_in_everything.begin(), tracks_not_in_everything.end() );
	std::random_shuffle( everything_playlist_iter+1, everything_playlist->track_ids.end(), playlistRandom );
	dao_factory->getPlaylistDAO()->insertOrUpdatePlaylist( everything_playlist );
}

void PlaylistService::exitMode() {
	switch( mode ) {
		case EVERYTHING:
			break;
		case ARTIST:
		case ALBUM_SHUFFLED:
		case ALBUM_ORDERED:
		case CUSTOM:
			if( current_playlist != NULL && current_playlist != everything_playlist ) {
				delete current_playlist;
			}
			break;
	}
	current_playlist = everything_playlist;
	current_playlist_iter = everything_playlist_iter;
	mode = EVERYTHING;
}

int PlaylistService::playlistRandom( int i ) {
	return std::rand() % i;
}

void PlaylistService::restoreState() {
	long everything_playlist_position;
	long current_playlist_position;
	long restore_mode_l;
	SettingsDAO* settings_dao = dao_factory->getSettingsDAO();

	settings_dao->get( SETTING_EVERYTHING_PLAYLIST_POSITION, everything_playlist_position );
	settings_dao->get( SETTING_CURRENT_PLAYLIST_POSITION, current_playlist_position );
	settings_dao->get( SETTING_PLAYLIST_SERVICE_MODE, restore_mode_l );

	if( everything_playlist->track_ids.end() - everything_playlist->track_ids.begin() <= everything_playlist_position ) {
		g_error( "Saved everythingPlaylistPosition is too large. Reverting to beginning" );
		everything_playlist_position = 0;
	}
	everything_playlist_iter = everything_playlist->track_ids.begin() + everything_playlist_position;

	mode = (PlaybackMode) restore_mode_l;

	if( mode != EVERYTHING ) {
		Playlist criterion;
		criterion.name.assign( SPECIAL_PLAYLIST_NAME );
		Playlist* new_playlist = dao_factory->getPlaylistDAO()->getPlaylist( &criterion );
		if( new_playlist == NULL ) {
			// TODO: report error
			current_playlist = everything_playlist;
			current_playlist_iter = everything_playlist_iter;
			mode = EVERYTHING;
		} else {
			current_playlist = new Playlist( *new_playlist );
			dao_factory->getPlaylistDAO()->free( new_playlist );
		}

		if( current_playlist->track_ids.end() - current_playlist->track_ids.begin() <= current_playlist_position ) {
			g_error( "Saved current_playlist_position is too large. Reverting to beginning" );
			current_playlist_position = 0;
		}
		current_playlist_iter = current_playlist->track_ids.begin() + current_playlist_position;
	} else {
		current_playlist = everything_playlist;
		current_playlist_iter = everything_playlist_iter;
	}
}

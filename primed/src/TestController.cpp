/*
 * TestController.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <exception>

#include "TestController.hpp"
#include "PlaylistService.hpp"
#include "MetadataService.hpp"

gboolean TestController::test_playlist_service = FALSE;
gboolean TestController::test_database = FALSE;
GOptionEntry TestController::options[] = {
		{ "test-database", 0, 0, G_OPTION_ARG_NONE, &test_database, "Test DAOFactory class", NULL },
		{ "test-playlist-service", 0, 0, G_OPTION_ARG_NONE, &test_playlist_service, "Test PlaylistService class", NULL },
		{ NULL }
};

TestController::TestController() :
		Controller() {
}

TestController::~TestController() {
	// TODO Auto-generated destructor stub
}

GOptionGroup* TestController::getOptionGroup() {
	GOptionGroup* group = g_option_group_new(
			"test",
			"Options for the Test Controller",
			"The Test Controller runs tests on various portions of the Gemini codebase",
			NULL,
			NULL );
	g_option_group_add_entries( group, options );
	return group;
}

void TestController::run( bool& allow_daemon ) {
	if( test_database ) {
		testDatabase();
		allow_daemon = false;
	}
	if( test_playlist_service ) {
		testPlaylistService();
		allow_daemon = false;
	}
}

void printCurrentTrackFromPlaylistService( PlaylistService* playlistService, DAOFactory* daoFactory ) {
	Track criterion;
	Track* track = NULL;
	TrackDAO* trackDAO = daoFactory->getTrackDAO();

	criterion.id = playlistService->getCurrentTrackId();
	track = trackDAO->getTrack( &criterion );
	track->print();
	trackDAO->free( track );
}

void TestController::testDatabase() {
	try {
		TrackDAO* trackDAO = dao_factory->getTrackDAO();
		Track criterion;
		criterion.id = 120;
		Track* track = trackDAO->getTrack( &criterion );
		track->print();
		trackDAO->free( track );
	} catch( std::exception* e ) {
		g_error( e->what() );
	}
}

void TestController::testPlaylistService() {
	try {
		PlaylistService* playlist_service = new PlaylistService( dao_factory );
		MetadataService* metadata_service = new MetadataService( dao_factory );
		Track* track = NULL;

		g_print( "-- Test PlaylistService::getCurrentTrackId\n" );
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cueNextTrack\n" );
		playlist_service->cueNextTrack();
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cueArtistById\n" );
		long artist_id = -1;
		do {
			track = metadata_service->getTrackById( playlist_service->getCurrentTrackId() );
			if( track->artist != NULL ) {
				artist_id = track->artist->id;
			} else {
				g_print( "-- Advancing\n");
				playlist_service->cueNextTrack();
				printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			}
			metadata_service->free( track );
		} while( artist_id < 0 );
		playlist_service->cueArtistById( artist_id );
		do {
			printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			playlist_service->cueNextTrack();
		} while( playlist_service->getPlaybackMode() == PlaylistService::ARTIST );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cueAlbumShuffledById\n" );
		long album_id = -1;
		do {
			track = metadata_service->getTrackById( playlist_service->getCurrentTrackId() );
			if( track->album != NULL ) {
				album_id = track->album->id;
			} else {
				g_print( "-- Advancing\n");
				playlist_service->cueNextTrack();
				printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			}
			metadata_service->free( track );
		} while( album_id < 0 );
		playlist_service->cueAlbumShuffledById( album_id );
		do {
			printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			playlist_service->cueNextTrack();
		} while( playlist_service->getPlaybackMode() == PlaylistService::ALBUM_SHUFFLED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cuePreviousTrack\n" );
		playlist_service->cuePreviousTrack();
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cueAlbumOrderedById\n" );
		album_id = -1;
		do {
			track = metadata_service->getTrackById( playlist_service->getCurrentTrackId() );
			if( track->album != NULL ) {
				album_id = track->album->id;
			} else {
				g_print( "-- Advancing\n");
				playlist_service->cueNextTrack();
				printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			}
			metadata_service->free( track );
		} while( album_id < 0 );
		playlist_service->cueAlbumOrderedById( album_id );
		do {
			printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			playlist_service->cueNextTrack();
		} while( playlist_service->getPlaybackMode() == PlaylistService::ALBUM_ORDERED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		g_print( "-- Test PlaylistService::cueAlbumShuffledById & PlaylistService::cueAlbumOrderedById\n" );
		album_id = -1;
		do {
			track = metadata_service->getTrackById( playlist_service->getCurrentTrackId() );
			if( track->album != NULL ) {
				album_id = track->album->id;
			} else {
				g_print( "-- Advancing\n");
				playlist_service->cueNextTrack();
				printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			}
			metadata_service->free( track );
		} while( album_id < 0 );
		playlist_service->cueAlbumShuffledById( album_id );
		playlist_service->cueAlbumOrderedById( album_id );
		playlist_service->saveState();
		do {
			printCurrentTrackFromPlaylistService( playlist_service, dao_factory );
			playlist_service->cueNextTrack();
		} while( playlist_service->getPlaybackMode() == PlaylistService::ALBUM_ORDERED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlist_service, dao_factory );

		delete playlist_service;
		delete metadata_service;
	} catch( std::exception* ex ) {
		g_critical( "Exception: %s", ex->what() );
	}
}

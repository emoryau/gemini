/*
 * PlaybackController.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include <ctime>
#include "PlaybackController.hpp"
#include "PlaylistService.hpp"
#include "MetadataService.hpp"

gboolean PlaybackController::foreground = false;
GOptionEntry PlaybackController::options[] = {
		{ "foreground", 'f', 0, G_OPTION_ARG_NONE, &foreground, "Keep daemon in foreground", NULL },
		{ NULL }
};

PlaybackController::PlaybackController() {
	// TODO Auto-generated constructor stub

}

PlaybackController::~PlaybackController() {
	// TODO Auto-generated destructor stub
}

GOptionGroup* PlaybackController::getOptionGroup() {
	GOptionGroup* group = g_option_group_new(
			"playback",
			"Options for the Playback Controller",
			"The Playback Controller plays music from the Gemini database",
			NULL,
			NULL );
	g_option_group_add_entries( group, options );
	return group;
}

void PlaybackController::run( bool& allow_daemon ) {
	if( !allow_daemon ) {
		return;
	}

	if( foreground ) {
		g_debug( "Running playback controller" );
		daemon();
	} else {
		g_debug( "Forking into background and running playback controller" );
		daemon();
	}
}

void PlaybackController::daemon() {
	PlaylistService* playlist_service = new PlaylistService( dao_factory );
	MetadataService* metadata_service = new MetadataService( dao_factory );

	while( 1 ) {
		g_debug( "Playing a tune" );

		long track_id = playlist_service->getCurrentTrackId();
		Track* current_track = metadata_service->getTrackById( track_id );
		current_track->print();
		metadata_service->free( current_track );

		timespec sleep_time = {2, 0 };
		nanosleep( &sleep_time, NULL );

		playlist_service->cueNextTrack();
		playlist_service->saveState();
	}

	delete metadata_service;
	delete playlist_service;
}

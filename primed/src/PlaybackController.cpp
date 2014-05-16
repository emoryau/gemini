/*
 * PlaybackController.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include "PlaybackController.hpp"


GOptionEntry PlaybackController::options[] = {
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
	if( allow_daemon ) {
		g_debug( "Forking into background and running playback controller" );
	}
}

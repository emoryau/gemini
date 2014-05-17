/*
 * MaintenenceController.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <exception>

#include "MaintenanceController.hpp"
#include "TagExtractor.hpp"
#include "MetadataService.hpp"
#include "Filesystem.hpp"
#include "PlaylistService.hpp"
#include "GeminiException.hpp"

gchar* MaintenanceController::scan_directory = NULL;
gboolean MaintenanceController::update_playlists = FALSE;

GOptionEntry MaintenanceController::options[] = {
		{ "scan-directory", 's', 0, G_OPTION_ARG_FILENAME, &scan_directory, "Scan directory M for compatible music", "M" },
		{ "playlist-update", 'p', 0, G_OPTION_ARG_NONE, &update_playlists, "Update playlist data", NULL },
		{ NULL }
};

const char* MaintenanceController::extension_blacklist[] = { ".jpg", ".cue", ".db", ".m3u", ".ini", ".sfv", ".pdf", ".log", ".txt", ".png", NULL };

MaintenanceController::~MaintenanceController() {
	// TODO Auto-generated destructor stub
}

MaintenanceController::MaintenanceController(): Controller() {
	// TODO Auto-generated constructor stub

}

GOptionGroup* MaintenanceController::getOptionGroup() {
	GOptionGroup* group = g_option_group_new(
			"maint",
			"Options for the Maintenance Controller",
			"The Maintenance Controller updates database entries and other miscellaneous upkeep",
			NULL,
			NULL );
	g_option_group_add_entries( group, options );
	return group;
}

void MaintenanceController::run( bool& allow_daemon ) {
	if( scan_directory != NULL ) {
		scanDirectory();
		allow_daemon = false;
	}
	if( update_playlists ) {
		updatePlaylists();
		allow_daemon = false;
	}
}

void MaintenanceController::scanDirectory() {
	Filesystem directory_crawler( scan_directory );
	TagExtractor extractor;
	MetadataService metadata_service;

	g_print( "Scanning %s\n", scan_directory );

	metadata_service.setDAOFactory( dao_factory );

	try {
		for (Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it) {
			// Parse file
			std::string filename = *it;

			if( isInBlacklist( filename ) )
				continue;

			extractor.readTags( filename.c_str() );
			metadata_service.insertOrUpdateExtractedTrack( extractor );
		}
	} catch( GeminiException* ex ) {
		g_critical( "Exception: %s", ex->what() );
	}
}

void MaintenanceController::updatePlaylists() {
	try {
		PlaylistService* playlistService = new PlaylistService( dao_factory );

		playlistService->refreshEverythingPlaylist();

		delete playlistService;
	} catch (std::exception* ex ) {
		g_error( ex->what() );
	}
}

bool MaintenanceController::isInBlacklist( const std::string& subject ) {
	const char** extension_comparator = extension_blacklist;
	while( *extension_comparator != NULL ) {
		if( subject.rfind( *extension_comparator ) != std::string::npos ) {
			return true;
		}
		extension_comparator++;
	}
	return false;
}

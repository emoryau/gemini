/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include <glib.h>
#include <stdlib.h>
#include <iostream>
#include <exception>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>

#include "Filesystem.hpp"
#include "TagExtractor.hpp"
#include "MetadataStore.hpp"
#include "PlaylistService.hpp"
#include "dao/DAOFactory.hpp"
#include "TestController.hpp"

static const char* extension_blacklist[] = { ".jpg", ".cue", ".db", ".m3u", ".ini", ".sfv", ".pdf", ".log", ".txt", ".png", NULL };
static const gchar* scan_filename_default = "/home/emoryau/testmusic";
static const gchar* database_URI_default = "sqlite3:///home/emoryau/test.sqlite";
static gchar* database_URI = NULL;
static gchar* scan_filename = NULL;
static gchar** action_commands = NULL;

static std::list<Controller*> controllers;

static GOptionEntry command_line_entries[] =
{
		{ "scan_filename", 's', 0, G_OPTION_ARG_FILENAME, scan_filename, "Set music scan directory M", "M" },
		{ "database_uri", 'd', 0, G_OPTION_ARG_FILENAME, &database_URI, "Set name of database to D", "D" },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &action_commands, "Action", NULL },
		{ NULL }
};

static void actionScan( void );
void actionPlaylist(void);
void actionTestDb(void);
void actionPlaylistServiceTest(void);

void printTrack( const Track* track );

struct ActionRow {
	const gchar* command_text;
	const gchar* help_description;
	void (*action_func)(void);
};
static ActionRow actions[] =
{
		{ "scan", "Scan music directory and update database", actionScan },
		{ "playlist", "Update base playlist in database", actionPlaylist },
		{ "test_db", "Test database", actionTestDb }, // TODO: remove this for production code
		{ "test_playlist_service", "Test PlaylistService", actionPlaylistServiceTest }, // TODO: remove this for production code
		{ NULL }
};

bool isInBlacklist( const std::string& subject ) {
	const char** extension_comparator = extension_blacklist;
	while( *extension_comparator != NULL ) {
		if( subject.rfind( *extension_comparator ) != std::string::npos ) {
			return true;
		}
		extension_comparator++;
	}
	return false;
}

void actionScan( void ) {
	Filesystem directory_crawler( scan_filename );
	TagExtractor extractor;
	MetadataStore store;
	DAOFactory* daoFactory = DAOFactory::CreateDAOFactoryFromURI( database_URI );

	g_print( "Scanning %s\n", scan_filename );

	store.setDAOFactory( daoFactory );

	for (Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it) {
		// Parse file
		std::string filename = *it;

		if( isInBlacklist( filename ) )
			continue;

		extractor.readTags( filename.c_str() );
		store.addExtractedTrack( extractor );
	}
}

void actionPlaylist(void) {
	try {
		DAOFactory* daoFactory = DAOFactory::CreateDAOFactoryFromURI( database_URI );
		PlaylistService* playlistService = new PlaylistService( daoFactory );

		playlistService->refreshEverythingPlaylist();

		delete playlistService;
		delete daoFactory;
	} catch (std::exception* ex ) {
		g_error( ex->what() );
	}
}

void actionTestDb(void) {
	try {
			DAOFactory* daoFactory = DAOFactory::CreateDAOFactoryFromURI( database_URI );
			daoFactory->setDBFile( database_URI );
			TrackDAO* trackDAO = daoFactory->getTrackDAO();
			Track criterion;
			criterion.id = 120;
			Track* track = trackDAO->getTrack( &criterion );
			printTrack( track );
			trackDAO->free( track );
			delete daoFactory;
		} catch( std::exception* e ) {
			g_error( e->what() );
		}
}

void printCurrentTrackFromPlaylistService( PlaylistService* playlistService, DAOFactory* daoFactory ) {
	Track criterion;
	Track* track = NULL;
	TrackDAO* trackDAO = daoFactory->getTrackDAO();

	criterion.id = playlistService->getCurrentTrackId();
	track = trackDAO->getTrack( &criterion );
	printTrack( track );
	trackDAO->free( track );
}

void actionPlaylistServiceTest(void) {
	try {
		DAOFactory* daoFactory = DAOFactory::CreateDAOFactoryFromURI( database_URI );
		PlaylistService* playlistService = new PlaylistService( daoFactory );
		Track* criterion;
		Track* track = NULL;
		TrackDAO* trackDAO = daoFactory->getTrackDAO();

		g_print( "-- Test PlaylistService::getCurrentTrackId\n" );
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cueNextTrack\n" );
		playlistService->cueNextTrack();
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cueArtistById\n" );
		long artist_id = -1;
		do {
			criterion = new Track();
			criterion->id = playlistService->getCurrentTrackId();
			track = trackDAO->getTrack( criterion );
			if( track->artist != NULL ) {
				artist_id = track->artist->id;
			}
			trackDAO->free( track );
			delete criterion;
		} while( artist_id < 0 );
		playlistService->cueArtistById( artist_id );
		do {
			printCurrentTrackFromPlaylistService( playlistService, daoFactory );
			playlistService->cueNextTrack();
		} while ( playlistService->getPlaybackMode() == PlaylistService::ARTIST );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cueAlbumShuffledById\n" );
		long album_id = -1;
		do {
			criterion = new Track();
			criterion->id = playlistService->getCurrentTrackId();
			track = trackDAO->getTrack( criterion );
			if( track->album != NULL ) {
				album_id = track->album->id;
			}
			trackDAO->free( track );
			delete criterion;
		} while( album_id < 0 );
		playlistService->cueAlbumShuffledById( album_id );
		do {
			printCurrentTrackFromPlaylistService( playlistService, daoFactory );
			playlistService->cueNextTrack();
		} while ( playlistService->getPlaybackMode() == PlaylistService::ALBUM_SHUFFLED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cuePreviousTrack\n" );
		playlistService->cuePreviousTrack();
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cueAlbumOrderedById\n" );
		album_id = -1;
		do {
			criterion = new Track();
			criterion->id = playlistService->getCurrentTrackId();
			track = trackDAO->getTrack( criterion );
			if( track->album != NULL ) {
				album_id = track->album->id;
			}
			trackDAO->free( track );
			delete criterion;
		} while( album_id < 0 );
		playlistService->cueAlbumOrderedById( album_id );
		do {
			printCurrentTrackFromPlaylistService( playlistService, daoFactory );
			playlistService->cueNextTrack();
		} while ( playlistService->getPlaybackMode() == PlaylistService::ALBUM_ORDERED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		g_print( "-- Test PlaylistService::cueAlbumShuffledById & PlaylistService::cueAlbumOrderedById\n" );
		album_id = -1;
		do {
			criterion = new Track();
			criterion->id = playlistService->getCurrentTrackId();
			track = trackDAO->getTrack( criterion );
			if( track->album != NULL ) {
				album_id = track->album->id;
			}
			trackDAO->free( track );
			delete criterion;
		} while( album_id < 0 );
		playlistService->cueAlbumShuffledById( album_id );
		playlistService->cueAlbumOrderedById( album_id );
		playlistService->saveState();
		do {
			printCurrentTrackFromPlaylistService( playlistService, daoFactory );
			playlistService->cueNextTrack();
		} while ( playlistService->getPlaybackMode() == PlaylistService::ALBUM_ORDERED );
		g_print( "-- Back to everything playlist\n" );
		printCurrentTrackFromPlaylistService( playlistService, daoFactory );

		delete playlistService;
		delete daoFactory;
	} catch (std::exception* ex ) {
		g_error( ex->what() );
	}
}


std::list<ActionRow*> parseCommandLine( int argc, char** argv ) {
	GOptionContext* context;
	GError* error = NULL;
	std::stringstream summary_stream;

	summary_stream << " Gemini Prime - music playback daemon" << std::endl << std::endl;
	summary_stream << "Actions available: (at least one required)" << std::endl;
	summary_stream << std::setfill(' ');
	for( int i_actions = 0; actions[i_actions].command_text != NULL; i_actions++ ) {
		summary_stream << std::endl
			<< std::setw(22) << actions[i_actions].command_text
			<< std::setw(-1) << " " << actions[i_actions].help_description;
	}

	context = g_option_context_new( "ACTION_1 ACTION_2 ACTION_N" );
	g_option_context_set_summary( context, summary_stream.str().c_str() );
	g_option_context_add_main_entries( context, command_line_entries, NULL );

	for( std::list<Controller*>::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		g_option_context_add_group( context, (*controller_iter)->getOptionGroup() );
	}

	if( !g_option_context_parse( context, &argc, &argv, &error ) ) {
		g_print( "option parsing failed: %s\n", error->message );
		exit( 1 );
	}

	std::list<ActionRow*> requested_actions;

	if( action_commands != NULL ) {
		gchar** current_action_command = action_commands;
		while( *current_action_command != NULL ) {
			int i = 0;
			while ( actions[i].action_func != NULL ) {
				if( g_str_has_prefix( actions[i].command_text, *current_action_command ) ) {
					requested_actions.push_back( &actions[i] );
					break;
				}
				i++;
			}
			current_action_command++;
		}
	}

	if( requested_actions.size() <= 0 ) {
		const gchar* help_message = g_option_context_get_help( context, TRUE, NULL );
		g_print( help_message );
	}

	g_option_context_free( context );

	return requested_actions;
}

int main( int argc, char** argv ) {
	gst_init( &argc, &argv );

	controllers.push_back( new TestController );
	std::list<ActionRow*> requested_actions = parseCommandLine( argc, argv );

	if( requested_actions.size() <= 0 ) {
		g_error( "No action selected\n" );
		exit( 1 );
	}

	if( database_URI == NULL ) {
		database_URI = g_strdup( database_URI_default );
	}
	if( scan_filename == NULL ) {
		scan_filename = g_strdup( scan_filename_default );
	}
	g_print( "Database filename: %s\n", database_URI );

	for( std::list<ActionRow*>::iterator action = requested_actions.begin(); action != requested_actions.end(); action++ ) {
		g_print( "Starting requested_actions: %s\n", (*action)->command_text );
		if( (*action)->action_func != NULL ) {
			(*action)->action_func();
		}
		g_print( "Finished %s\n", (*action)->command_text );
	}

	g_free( database_URI );
	g_free( scan_filename );

	return 0;
}

void printTrack( const Track* track ) {
	g_print( "%35s '%-25s' - %d.%-2d %s\n", track->artist ? track->artist->name.c_str() : "<no artist>",
			track->album ? track->album->name.c_str() : "<no album>", track->discNumber, track->trackNumber, track->name.c_str() );
}
void printExtractorContents( const TagExtractor& extractor ) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t"
			<< extractor.getTrackGain() << "\n";
}

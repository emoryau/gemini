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

typedef std::list<Controller*> ControllerList;
static ControllerList controllers;

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
			track->print();
			trackDAO->free( track );
			delete daoFactory;
		} catch( std::exception* e ) {
			g_error( e->what() );
		}
}



void parseCommandLine( int argc, char** argv ) {
	GOptionContext* context;
	GError* error = NULL;

	context = g_option_context_new( "- music playback daemon" );
	g_option_context_add_main_entries( context, command_line_entries, NULL );

	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		g_option_context_add_group( context, (*controller_iter)->getOptionGroup() );
	}

	if( !g_option_context_parse( context, &argc, &argv, &error ) ) {
		g_error( "option parsing failed: %s", error->message );
		exit( 1 );
	}

	g_option_context_free( context );
}

int main( int argc, char** argv ) {
	gst_init( &argc, &argv );

	controllers.push_back( new TestController );

	parseCommandLine( argc, argv );

	if( database_URI == NULL ) {
		database_URI = g_strdup( database_URI_default );
	}
	if( scan_filename == NULL ) {
		scan_filename = g_strdup( scan_filename_default );
	}

	DAOFactory* dao_factory = DAOFactory::CreateDAOFactoryFromURI( database_URI );
	if( dao_factory == NULL ) {
		g_error( "Could not create DAOFactory from URI: %s", database_URI );
		return 1;
	}

	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		(*controller_iter)->setDAOFactory( dao_factory );
	}


	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		(*controller_iter)->run();
	}

	delete dao_factory;
	g_free( database_URI );
	g_free( scan_filename );

	return 0;
}

void printExtractorContents( const TagExtractor& extractor ) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t"
			<< extractor.getTrackGain() << "\n";
}

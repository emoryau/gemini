/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"
#include "TagExtractor.hpp"
#include "MetadataStore.hpp"
#include "dao/sqlite3/DAOFactorySqlite3Impl.hpp"
#include <glib.h>
#include <stdlib.h>
#include <iostream>
#include <exception>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>

static const char* extension_blacklist[] = { ".jpg", ".cue", ".db", ".m3u", ".ini", ".sfv", ".pdf", ".log", ".txt", ".png", NULL };
static const gchar* scan_filename = "/home/emoryau/testmusic";
static const gchar* database_filename = "/home/emoryau/test.sqlite";
static gchar** action_commands = NULL;

static GOptionEntry command_line_entries[] =
{
		{ "scan", 's', 0, G_OPTION_ARG_FILENAME, &scan_filename, "Scan directory M for music, then exit", "M" },
		{ "database", 'd', 0, G_OPTION_ARG_FILENAME, &database_filename, "Set name of database to D", "D" },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &action_commands, "Action", NULL },
		{ NULL }
};

static void actionScan( void );
void actionPlaylist(void);
void actionDbTest(void);

void printTrack( const Track* track );

struct ActionRow {
	const gchar* command_text;
	const gchar* help_description;
	void (*action_func)(void);
};
static ActionRow actions[] =
{
		{ "scan", "Scan music directory and update database", actionScan },
		{ "db_test", "Test database", actionDbTest }, // TODO: remove this for production code
		{ "playlist", "Update base playlist in database", actionPlaylist },
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
	DAOFactorySqlite3Impl* daoFactory;

	g_print( "Scanning %s\n", scan_filename );

	// Create some DAO's to talk to the db
	daoFactory = new DAOFactorySqlite3Impl();
	daoFactory->setDBFile( database_filename );
	store.setDAOFactory( daoFactory );

	for (Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it) {
		// Parse file
		std::string filename = *it;

		if( isInBlacklist( filename ) )
			continue;

		extractor.readTags( filename.c_str() );
		try {
			store.addExtractedTrack( extractor );
		} catch( MetadataStoreException* e ) {
			g_error( "addExtractedTrack() threw:\n%s", e->what() );
		}
	}
}

void actionPlaylist(void) {
	DAOFactorySqlite3Impl* daoFactory = new DAOFactorySqlite3Impl();
	daoFactory->setDBFile( database_filename );
	TrackDAO* trackDAO = daoFactory->getTrackDAO();
	ArtistDAO* artistDAO = daoFactory->getArtistDAO();
	AlbumDAO* albumDAO = daoFactory->getAlbumDAO();

	{
		Playlist* all_tracks = trackDAO->getTrackIds();
		for( Playlist::iterator iter_playlist = all_tracks->begin(); iter_playlist != all_tracks->end(); iter_playlist++) {
			g_print( "%d\t", (*iter_playlist) );
		}
		g_print( "\n" );
		trackDAO->free( all_tracks );
		all_tracks = NULL;
	}

	try {
		Artist criterion;
		criterion.name.assign( "Adele" );
		Artist* artist = artistDAO->getArtist( &criterion );
		g_print( "Artist test - %s\n", artist->name.c_str() );
		Playlist* artist_tracks = trackDAO->getTrackIdsByArtist( artist->id );
		for( Playlist::iterator iter_playlist = artist_tracks->begin(); iter_playlist != artist_tracks->end(); iter_playlist++) {
			Track criterion;
			criterion.id = *iter_playlist;
			Track* track = trackDAO->getTrack( &criterion );
			if( track ) {
				printTrack( track );
				trackDAO->free( track );
			}
		}
		trackDAO->free( artist_tracks );
		artist_tracks = NULL;
	} catch (std::exception* ex ) {
		g_error( ex->what() );
	}

	{
		Album criterion;
		criterion.name.assign( "21" );
		Album* album = albumDAO->getAlbum( &criterion );
		g_print( "Album test - %s\n", album->name.c_str() );
		Playlist* album_tracks = trackDAO->getTrackIdsByAlbum( album->id );
		for( Playlist::iterator iter_playlist = album_tracks->begin(); iter_playlist != album_tracks->end(); iter_playlist++) {
			Track criterion;
			criterion.id = *iter_playlist;
			Track* track = trackDAO->getTrack( &criterion );
			if( track ) {
				printTrack( track );
				trackDAO->free( track );
			}
		}
		trackDAO->free( album_tracks );
		album_tracks = NULL;
	}

	delete daoFactory;
}

void actionDbTest(void) {
	try {
			DAOFactorySqlite3Impl* daoFactory = new DAOFactorySqlite3Impl();
			daoFactory->setDBFile( database_filename );
			TrackDAO* trackDAO = daoFactory->getTrackDAO();
			Track criterion;
			criterion.id = 120;
			Track* track = trackDAO->getTrack( &criterion );
			g_print( "%s - '%s' - %d.%d - %s\n", track->artist ? track->artist->name.c_str() : "<no artist>",
					track->album ? track->album->name.c_str() : "<no album>", track->discNumber, track->trackNumber, track->name.c_str() );
			trackDAO->free( track );
			delete daoFactory;
		} catch( std::exception* e ) {
			g_error( e->what() );
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

	std::list<ActionRow*> requested_actions = parseCommandLine( argc, argv );

	if( requested_actions.size() <= 0 ) {
		g_error( "No action selected\n" );
		exit( 1 );
	}

	g_print( "Database filename: %s\n", database_filename );

	for( std::list<ActionRow*>::iterator action = requested_actions.begin(); action != requested_actions.end(); action++ ) {
		g_print( "Starting requested_actions: %s\n", (*action)->command_text );
		if( (*action)->action_func != NULL ) {
			(*action)->action_func();
		}
		g_print( "Finished %s\n", (*action)->command_text );
	}

	return 0;
}

void printTrack( const Track* track ) {
	g_print( "%s - '%s' - %d.%d - %s\n", track->artist ? track->artist->name.c_str() : "<no artist>",
			track->album ? track->album->name.c_str() : "<no album>", track->discNumber, track->trackNumber, track->name.c_str() );
}
void printExtractorContents( const TagExtractor& extractor ) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t"
			<< extractor.getTrackGain() << "\n";
}

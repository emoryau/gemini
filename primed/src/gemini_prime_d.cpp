/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include <glib.h>
#include <gst/gst.h>
#include <string>
#include <list>
#include <cstdlib>
#include <ctime>

#include "dao/DAOFactory.hpp"
#include "TestController.hpp"
#include "MaintenanceController.hpp"
#include "PlaybackController.hpp"

static const gchar* database_URI_default = "sqlite3:///home/emoryau/test.sqlite";
static gchar* database_URI = NULL;

typedef std::list<Controller*> ControllerList;
static ControllerList controllers;

static GOptionEntry command_line_entries[] =
{
		{ "database-uri", 'd', 0, G_OPTION_ARG_FILENAME, &database_URI, "Set name of database to D", "D" },
		{ NULL }
};


void parseCommandLine( int argc, char** argv ) {
	GOptionContext* context;
	GError* error = NULL;

	context = g_option_context_new( "- music playback daemon" );
	g_option_context_add_main_entries( context, command_line_entries, NULL );

	g_option_context_add_group (context, gst_init_get_option_group ());
	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		g_option_context_add_group( context, (*controller_iter)->getOptionGroup() );
	}

	if( !g_option_context_parse( context, &argc, &argv, &error ) ) {
		g_error( "option parsing failed: %s", error->message );
		std::exit( 1 );
	}

	g_option_context_free( context );
}

int main( int argc, char** argv ) {
	std::srand( unsigned ( std::time(0) ) );
	gst_init( NULL, NULL );

	controllers.push_back( new TestController );
	controllers.push_back( new MaintenanceController );
	// Important for playback to be last to ensure daemon only runs when no other actions are specified
	controllers.push_back( new PlaybackController );

	parseCommandLine( argc, argv );

	if( database_URI == NULL ) {
		database_URI = g_strdup( database_URI_default );
	}

	DAOFactory* dao_factory = DAOFactory::CreateDAOFactoryFromURI( database_URI );
	if( dao_factory == NULL ) {
		g_error( "Could not create DAOFactory from URI: %s", database_URI );
		return 1;
	}

	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		(*controller_iter)->setDAOFactory( dao_factory );
	}

	bool allow_daemon = true;
	for( ControllerList::iterator controller_iter = controllers.begin(); controller_iter != controllers.end(); controller_iter++ ) {
		(*controller_iter)->run( allow_daemon );
	}

	delete dao_factory;
	g_free( database_URI );

	return 0;
}

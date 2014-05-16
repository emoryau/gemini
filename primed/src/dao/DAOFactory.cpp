/*
 * DAOFactory.cpp
 *
 *  Created on: May 16, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <string.h>
#include "DAOFactory.hpp"
#include "sqlite3/DAOFactorySqlite3Impl.hpp"


DAOFactory* DAOFactory::CreateDAOFactoryFromURI( const char* db_uri ) {
	DAOFactory* daoFactory = NULL;

	char* scheme = g_uri_parse_scheme( db_uri );
	char* filename = g_strdup( db_uri + strlen( scheme ) + 3 );

	if( g_ascii_strcasecmp( "sqlite3", scheme ) == 0 ) {
		daoFactory = new DAOFactorySqlite3Impl();
		daoFactory->setDBFile( filename );
	}

	g_free( scheme );
	g_free( filename );
	return daoFactory;
}


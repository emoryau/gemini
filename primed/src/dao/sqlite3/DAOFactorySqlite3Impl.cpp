/*
 * DAOServiceSqlite3Impl.cpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#include "DAOFactorySqlite3Impl.hpp"
#include "TrackDAOSqlite3Impl.hpp"

DAOFactorySqlite3Impl::DAOFactorySqlite3Impl() {
	db = NULL;
	trackDAO = NULL;
	artistDAO = NULL;
	albumDAO = NULL;
}

DAOFactorySqlite3Impl::~DAOFactorySqlite3Impl() {
	if( trackDAO != NULL ) {
		delete trackDAO;
		trackDAO = NULL;
	}
	if( artistDAO != NULL ) {
		delete artistDAO;
		artistDAO = NULL;
	}
	if( albumDAO != NULL ) {
		delete albumDAO;
		albumDAO = NULL;
	}
	if( db != NULL ) {
		sqlite3_close( db );
		db = NULL;
	}
}

void DAOFactorySqlite3Impl::SetDBFile( const char* filename ) {
	int rc;

	rc = sqlite3_open( filename, &db );
	if( rc != SQLITE_OK ) {
		sqlite3_close( db );
		db = NULL;
		// Throw DB exception
		throw new Sqlite3Exception( "Error opening db" );
	}

	trackDAO = new TrackDAOSqlite3Impl( db, artistDAO, albumDAO );
	trackDAO->ensureDBSchema();
}

/*
 * DAOServiceSqlite3Impl.cpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#include "DAOFactorySqlite3Impl.hpp"
#include "ArtistDAOSqlite3Impl.hpp"
#include "AlbumDAOSqlite3Impl.hpp"
#include "TrackDAOSqlite3Impl.hpp"
#include "PlaylistDAOSqlite3Impl.hpp"
#include "GeminiException.hpp"


DAOFactorySqlite3Impl::DAOFactorySqlite3Impl(void) {
	db = NULL;
	trackDAO = NULL;
	artistDAO = NULL;
	albumDAO = NULL;
	playlistDAO = NULL;
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
	if( playlistDAO != NULL ) {
		delete playlistDAO;
		playlistDAO = NULL;
	}
	if( db != NULL ) {
		sqlite3_close( db );
		db = NULL;
	}
}

void DAOFactorySqlite3Impl::setDBFile( const char* filename ) {
	int rc;

	rc = sqlite3_open( filename, &db );
	if( rc != SQLITE_OK ) {
		sqlite3_close( db );
		db = NULL;
		// Throw DB exception
		THROW_GEMINI_EXCEPTION( "Error opening db" );
	}

	artistDAO = new ArtistDAOSqlite3Impl( db );
	artistDAO->ensureDBSchema();
	albumDAO = new AlbumDAOSqlite3Impl( db, artistDAO );
	albumDAO->ensureDBSchema();
	trackDAO = new TrackDAOSqlite3Impl( db, artistDAO, albumDAO );
	trackDAO->ensureDBSchema();
	playlistDAO = new PlaylistDAOSqlite3Impl( db );
	playlistDAO->ensureDBSchema();
}

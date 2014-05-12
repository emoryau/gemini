/*
 * AlbumDAOSqlite3Impl.cpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#include "AlbumDAOSqlite3Impl.hpp"
#include <glib.h>
#include <sstream>

AlbumDAOSqlite3Impl::AlbumDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO ) {
	this->db = db;
	this->artistDAO = artistDAO;
}

AlbumDAOSqlite3Impl::~AlbumDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

void AlbumDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	checkDb();

	sql.append( "CREATE TABLE IF NOT EXISTS `Albums` (" );
	sql.append( "  `AlbumId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Name` VARCHAR(255) UNIQUE NOT NULL," );
	sql.append( "  `ReplayGain` FLOAT" );
	sql.append( ");" );

	rc = sqlite3_exec( db, sql.c_str(), NULL, 0, &zErrMsg );
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

void AlbumDAOSqlite3Impl::free( Album* album ) {
	if( album->artist != NULL ) {
		artistDAO->free( album->artist );
		album->artist = NULL;
	}
	delete album;
}

Album* AlbumDAOSqlite3Impl::getAlbum( Album* criterion ) {
	Album* album;
	int column = 0;
	std::stringstream sql;
	bool included_where = false;

	checkDb();

	sql << "SELECT * FROM `Albums`";
	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			sql << (included_where ? " AND " : " WHERE ");
			sql << "`AlbumId` = :albumid";
			included_where = true;
		}
		if( !criterion->name.empty() ) {
			sql << (included_where ? " AND " : " WHERE ");
			sql << "`Name` = :name";
			included_where = true;
		}
		if( criterion->replayGain > -99.0 ) {
			sql << (included_where ? " AND " : " WHERE ");
			sql << "`ReplayGain` = :replaygain";
			included_where = true;
		}
	}
	sql << ';';

	sqlite3_stmt* pStmt = prepare( sql.str().c_str() );

	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			bindLong( pStmt, ":albumid", criterion->id );
		}
		if( !criterion->name.empty() ) {
			bindText( pStmt, ":name", criterion->name.c_str() );
		}
		if( criterion->replayGain > -99.0 ) {
			bindDouble( pStmt, ":replaygain", criterion->replayGain );
		}
	}

	if( step( pStmt ) != SQLITE_ROW ) {
		return NULL;
	}

	album = new Album();
	album->id = sqlite3_column_int64( pStmt, column++ );
	album->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
	album->replayGain = sqlite3_column_double( pStmt, column++ );

	finalize( pStmt );

	return album;
}

void AlbumDAOSqlite3Impl::insertOrUpdateAlbum( Album* album ) {
	sqlite3_stmt *pStmt = NULL;

	album->id = -1;
	checkDb();

	pStmt = prepare( "SELECT `AlbumId`, `ReplayGain` FROM `Albums` WHERE `Name` = :name;" );
	bindText( pStmt, ":name", album->name.c_str() );
	if( step( pStmt ) == SQLITE_ROW ) {
		album->id = sqlite3_column_int64( pStmt, 0 );
		double replayGain = sqlite3_column_double( pStmt, 1 );
		if( replayGain != album->replayGain && album->name.compare("") == 0 ) {
			// TODO: Determine course of action
			g_print( "Found album '%s', with different ReplayGain (%f, %f)\n", album->name.c_str(), album->replayGain, replayGain );
		}
	}
	finalize( pStmt );

	if( album->id < 0 ) {
		pStmt = prepare(
				"INSERT INTO `Albums`"
				"(`Name`, `ReplayGain`)"
				"values(:name, :replay_gain);" );
		bindText( pStmt, ":name", album->name.c_str() );
		bindDouble( pStmt, ":replay_gain", album->replayGain );
		step( pStmt );
		finalize( pStmt );
		album->id = sqlite3_last_insert_rowid( db );
	}
}

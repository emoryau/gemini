/*
 * AlbumDAOSqlite3Impl.cpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <string>
#include "AlbumDAOSqlite3Impl.hpp"
#include "GeminiException.hpp"

AlbumDAOSqlite3Impl::AlbumDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO ): BaseSqlite3Impl( db ), artistDAO( artistDAO ) {};

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
		THROW_GEMINI_EXCEPTION( zErrMsg );
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
	Album* album = NULL;
	int column = 0;
	QueryCriteriaList queryCriteriaList;
	std::string* sql;

	checkDb();

	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			QueryCriteria qc = {"AlbumId", ":albumid", QueryCriteria::LONG};
			qc.value = &criterion->id;
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->name.empty() ) {
			QueryCriteria qc = {"Name", ":name", QueryCriteria::TEXT};
			qc.value = criterion->name.c_str();
			queryCriteriaList.push_back(qc);
		}
		if( criterion->replayGain > -99.0 ) {
			QueryCriteria qc = {"ReplayGain", ":replaygain", QueryCriteria::DOUBLE};
			qc.value = &criterion->replayGain;
			queryCriteriaList.push_back(qc);
		}
	}

	sql = buildSqlFromQueryCriteria( "Albums", queryCriteriaList );
	sqlite3_stmt* pStmt = prepare( sql->c_str() );
	bindVariablesFromQueryCriteria( pStmt, queryCriteriaList );

	if( step( pStmt ) == SQLITE_ROW ) {
		album = new Album();
		album->id = sqlite3_column_int64( pStmt, column++ );
		album->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
		album->replayGain = sqlite3_column_double( pStmt, column++ );
	}

	finalize( pStmt );
	delete sql;

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
		if( replayGain != album->replayGain && album->name.compare("") != 0 ) {
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

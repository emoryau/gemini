/*
 * ArtistDAOSqlite3Impl.cpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#include "ArtistDAOSqlite3Impl.hpp"
#include <string>
#include <glib.h>

ArtistDAOSqlite3Impl::ArtistDAOSqlite3Impl( sqlite3* db ) {
	this->db = db;
}

ArtistDAOSqlite3Impl::~ArtistDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

void ArtistDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	checkDb();

	sql.append( "CREATE TABLE IF NOT EXISTS `Artists` (" );
	sql.append( "`ArtistId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Name` VARCHAR(255) UNIQUE NOT NULL" );
	sql.append( ");" );

	rc = sqlite3_exec( db, sql.c_str(), NULL, 0, &zErrMsg );
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

void ArtistDAOSqlite3Impl::free( Artist* artist ) {
	delete artist;
}

Artist* ArtistDAOSqlite3Impl::getArtist( Artist* criterion ) {
	Artist* artist;
	int column = 0;
	QueryCriteriaList queryCriteriaList;
	std::string* sql;

	checkDb();

	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			QueryCriteria qc = {"ArtistId", ":artistid", QueryCriteria::LONG, &criterion->id};
			qc.value = &criterion->id;
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->name.empty() ) {
			QueryCriteria qc = {"Name", ":name", QueryCriteria::TEXT};
			qc.value = criterion->name.c_str();
			queryCriteriaList.push_back(qc);
		}
	}

	sql = buildSqlFromQueryCriteria( "Artists", queryCriteriaList );
	sqlite3_stmt* pStmt = prepare( sql->c_str() );
	bindVariablesFromQueryCriteria( pStmt, queryCriteriaList );

	if( step( pStmt ) != SQLITE_ROW ) {
		return NULL;
	}

	artist = new Artist();
	artist->id = sqlite3_column_int64( pStmt, column++ );
	artist->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );

	finalize( pStmt );
	delete sql;

	return artist;
}

void ArtistDAOSqlite3Impl::insertOrUpdateArtist( Artist* artist ) {
	sqlite3_stmt* pStmt = NULL;

	artist->id = -1;
	checkDb();

	pStmt = prepare( "SELECT `ArtistId` FROM `Artists` WHERE `Name` = :name;" );
	bindText( pStmt, ":name", artist->name.c_str() );
	if( step( pStmt ) == SQLITE_ROW ) {
		artist->id = sqlite3_column_int64( pStmt, 0 );
	}
	finalize( pStmt );

	if( artist->id < 0 ) {
		pStmt = prepare(
				"INSERT INTO `Artists`"
				"(`Name`)"
				"values(:name);");
		bindText( pStmt, ":name", artist->name.c_str() );
		step( pStmt );
		finalize( pStmt );
		artist->id = sqlite3_last_insert_rowid( db );
	}
}

/*
 * ArtistDAOSqlite3Impl.cpp
 *
 *  Created on: May 9, 2014
 *      Author: emory.au
 */

#include "ArtistDAOSqlite3Impl.hpp"

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

Artist* ArtistDAOSqlite3Impl::getArtistById( long id ) {
	Artist* artist;
	int column = 0;

	checkDb();

	sqlite3_stmt* pStmt = prepare( "SELECT "
			"`Name`"
			" FROM `Artists` WHERE `ArtistId` = :artistid" );
	bindLong( pStmt, ":artistid", id );
	if( step( pStmt ) != SQLITE_ROW ) {
		return NULL;
	}

	artist = new Artist();
	artist->id = id;
	artist->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );

	finalize( pStmt );

	return artist;
}

void ArtistDAOSqlite3Impl::free( Artist* artist ) {
	delete artist;
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

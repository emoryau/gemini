/*
 * MetadataStore.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#include <stdio.h>
#include "MetadataStore.hpp"

MetadataStore::MetadataStore() {
	db = NULL;
}

void MetadataStore::open( const char* db_filename ) {
	int rc;

	rc = sqlite3_open( db_filename, &db );
	if( rc != SQLITE_OK ) {
		sqlite3_close( db );
		db = NULL;
		// Throw DB exception
		throw new MetadataStoreException( "Error opening db" );
	}

	ensureDBSchema();
}
MetadataStore::~MetadataStore() {
	if( db != NULL ) {
		sqlite3_close( db );
		db = NULL;
	}
}

void MetadataStore::AddTrack( Track* track ) {
	// TODO: Add track, album, artist information to the store
}

void MetadataStore::addExtractedTrack( TagExtractor& te ) {
	Track* track = new Track( te );
	Album* album = new Album( te );
	Artist* artist = new Artist( te );
	
	AddArtist( artist );
	AddAlbum( album );
}

void MetadataStore::bindText( sqlite3_stmt* ppStmt, const char* field, const char* text ) {
	int rc;
	rc = sqlite3_bind_text( ppStmt,
			sqlite3_bind_parameter_index( ppStmt, field ),
			text,	
			-1,
			SQLITE_STATIC );
	if( rc != SQLITE_OK ) {
		throw new MetadataStoreException( sqlite3_errstr(rc) );
	}
}

void MetadataStore::bindDouble( sqlite3_stmt* ppStmt, const char* field, const double d ) {
	int rc;
	rc = sqlite3_bind_double( ppStmt,
			sqlite3_bind_parameter_index( ppStmt, field ),
			d);
	if( rc != SQLITE_OK ) {
		throw new MetadataStoreException( sqlite3_errstr(rc) );
	}
}

sqlite3_stmt* MetadataStore::prepare( const char* sql ) {
	int rc;
	sqlite3_stmt *ppStmt = NULL;

	rc = sqlite3_prepare_v2( db, sql, -1, &ppStmt, NULL);
	if( rc != SQLITE_OK ) {
		throw new MetadataStoreException( sqlite3_errmsg( db ) );
	}
	return ppStmt;
}

void MetadataStore::finalize( sqlite3_stmt* ppStmt ) {
	int rc;
	rc = sqlite3_finalize( ppStmt );
	if( rc != SQLITE_OK ) {
		throw new MetadataStoreException( sqlite3_errmsg( db ) );
	}
}

void MetadataStore::checkDb() {
	if( db == NULL ) {
		throw new MetadataStoreException( "DB not initialized" );
	}
}

void MetadataStore::step( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_step( pStmt );
	if( rc != SQLITE_DONE ) {
		throw new MetadataStoreException( sqlite3_errmsg( db ) );
	}
}

sqlite3_int64 MetadataStore::AddArtist( Artist* artist ) {
	const char* sql =
		"INSERT OR REPLACE INTO `Artists`"
		"(`Name`)"
		 "values(:name);";
	sqlite3_stmt *ppStmt = NULL;

	checkDb();

	ppStmt = prepare( sql );
	bindText( ppStmt, ":name", artist->name.c_str() );
	step( ppStmt );
	finalize( ppStmt );
	return( sqlite3_last_insert_rowid( db ) );
}

sqlite3_int64 MetadataStore::AddAlbum( Album* album ) {
	const char* sql =
		"INSERT OR REPLACE INTO `Albums`"
		"(`Name`, `ReplayGain`)"
		"values(:name, :replay_gain);";
	sqlite3_stmt *ppStmt = NULL;

	checkDb();

	ppStmt = prepare( sql );
	bindText( ppStmt, ":name", album->name.c_str() );
	bindDouble( ppStmt, ":replay_gain", album->replayGain );
	step( ppStmt );
	finalize( ppStmt );
	return( sqlite3_last_insert_rowid( db ) );
}

void MetadataStore::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	if( db == NULL ) {
		throw new MetadataStoreException( "DB not initialized" );
	}

	sql.append( "CREATE TABLE IF NOT EXISTS `Artists` (" );
	sql.append( "`  ArtistId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Name` VARCHAR(255) UNIQUE NOT NULL" );
	sql.append( ");" );

	sql.append( "CREATE TABLE IF NOT EXISTS `Albums` (" );
	sql.append( "  `AlbumId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Name` VARCHAR(255) UNIQUE NOT NULL," );
	sql.append( "  `ReplayGain` FLOAT" );
	sql.append( ");" );

	sql.append( "CREATE TABLE IF NOT EXISTS `Tracks` (" );
	sql.append( "  `TrackId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Filename` VARCHAR(255) NOT NULL," );
	sql.append( "  `Checksum` VARCHAR(32) NOT NULL," );
	sql.append( "  `Name` VARCHAR(255) NOT NULL," );
	sql.append( "  `Art_Filename` VARCHAR(255) DEFAULT NULL," );
	sql.append( "  `ArtistId` INTEGER NOT NULL," );
	sql.append( "  `AlbumArtistId` INTEGER DEFAULT NULL," );
	sql.append( "  `AlbumId` INTEGER DEFAULT NULL," );
	sql.append( "  `TrackNumber` INTEGER DEFAULT 0," );
	sql.append( "  `ReplayGain` FLOAT," );
	sql.append( "  FOREIGN KEY(`ArtistId`) REFERENCES `Artists`(`ArtistId`) ON DELETE RESTRICT," );
	sql.append( "  FOREIGN KEY(`AlbumArtistId`) REFERENCES `Artists`(`ArtistId`) ON DELETE RESTRICT," );
	sql.append( "  FOREIGN KEY(`AlbumId`) REFERENCES `Albums`(`AlbumId`) ON DELETE RESTRICT" );
	sql.append( ");" );

	rc = sqlite3_exec( db, sql.c_str(), NULL, 0, &zErrMsg );
	if( rc != SQLITE_OK ) {
		throw new MetadataStoreException( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

MetadataStoreException::MetadataStoreException( const char* what ) throw() {
	cause.assign( what );
}

const char* MetadataStoreException::what() const throw() {
	return cause.c_str();
}


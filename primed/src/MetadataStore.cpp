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
		throw "Error opening db";
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

void MetadataStore::AddExtractedTrack( TagExtractor& te ) {
	Track* track = new Track( te );
	Album* album = new Album( te );
	Artist* artist = new Artist( te );
}

void MetadataStore::AddArtist( Artist* artist ) {
}

void MetadataStore::AddAlbum( Album* album ) {
}

void MetadataStore::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	if( db == NULL ) {
		throw "DB not initialized";
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
		throw zErrMsg;
		sqlite3_free( zErrMsg );
	}
}

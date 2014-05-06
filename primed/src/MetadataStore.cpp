/*
 * MetadataStore.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#include "MetadataStore.hpp"
#include <glib.h>

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

void MetadataStore::addTrack( Track* track ) {
	sqlite3_stmt *pStmt = NULL;
	bool rowExists = false;

	checkDb();

	pStmt = prepare( "SELECT `Checksum` FROM `Tracks` WHERE `Filename` = :name;" );
	bindText( pStmt, ":name", track->filename.c_str() );
	if( step( pStmt ) == SQLITE_ROW ) {
		rowExists = true;
		//TODO: Consider checking this file's checksum
		//checksum.assign( sqlite3_column_text( pStmt, 0 ) );
	}
	finalize( pStmt );

	if( rowExists) {
		pStmt = prepare(
				"UPDATE `Tracks` SET "
				"`Checksum` = :checksum,"
				"`Name` = :name,"
				"`Art_Filename` = :art_filename,"
				"`ArtistId` = :artist_id,"
				"`AlbumArtistId` = :album_artist_id,"
				"`AlbumId` = :album_id,"
				"`TrackNumber` = :track_number,"
				"`DiscNumber` = :disc_number,"
				"`ReplayGain` = :replay_gain "
				"WHERE `Filename` = :filename" );
	} else {
		pStmt = prepare( 
				"INSERT INTO `Tracks`"
				"(`Filename`, `Checksum`, `Name`, `Art_Filename`, `ArtistId`, `AlbumArtistId`, `AlbumId`, `TrackNumber`,`DiscNumber`, `ReplayGain`)"
				"values(:filename, :checksum, :name, :art_filename, :artist_id, :album_artist_id, :album_id, :track_number, :disc_number, :replay_gain);" );
	}
	bindText( pStmt, ":filename", track->filename.c_str() );
	bindText( pStmt, ":checksum", "nop" );
	bindText( pStmt, ":name", track->name.c_str() );
	bindText( pStmt, ":art_filename", track->artFilename.c_str() );
	if( track->artist != NULL )
		bindLong( pStmt, ":artist_id", track->artist->id );
	if( track->album != NULL ) {
		bindLong( pStmt, ":album_id", track->album->id );
		if( track->album->artist != NULL ) {
			bindLong( pStmt, ":album_artist_id", track->album->artist->id );
		}
	}
	bindLong( pStmt, ":track_number", track->trackNumber );
	bindLong( pStmt, ":disc_number", track->discNumber );
	bindDouble( pStmt, ":replay_gain", track->replayGain );
	step( pStmt );
	finalize( pStmt );
}

void MetadataStore::addExtractedTrack( TagExtractor& te ) {
	Track* track = new Track( te );
	Album* album = new Album( te );
	Artist* artist = new Artist( te );
	
	album->artist = artist;
	track->album = album;
	track->artist = artist;

	addArtist( artist );
	addAlbum( album );
	addTrack( track );

	delete track;
	delete album;
	delete artist;
}

void MetadataStore::bindText( sqlite3_stmt* pStmt, const char* field, const char* text ) {
	int rc;
	rc = sqlite3_bind_text( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
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

void MetadataStore::bindLong( sqlite3_stmt* ppStmt, const char* field, const long l ) {
	int rc;
	rc = sqlite3_bind_int64( ppStmt,
			sqlite3_bind_parameter_index( ppStmt, field ),
			l);
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

int MetadataStore::step( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_step( pStmt );
	if( rc != SQLITE_DONE && rc != SQLITE_ROW ) {
		throw new MetadataStoreException( sqlite3_errmsg( db ) );
	}
	return rc;
}

long MetadataStore::addArtist( Artist* artist ) {
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

	return( artist->id );
}

long MetadataStore::addAlbum( Album* album ) {
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

	return( album->id );
}

void MetadataStore::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	if( db == NULL ) {
		throw new MetadataStoreException( "DB not initialized" );
	}

	sql.append( "CREATE TABLE IF NOT EXISTS `Artists` (" );
	sql.append( "`ArtistId` INTEGER PRIMARY KEY AUTOINCREMENT," );
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
	sql.append( "  `DiscNumber` INTEGER DEFAULT 0," );
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


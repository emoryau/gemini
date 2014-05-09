/*
 * TrackDAOSqlite3Impl.cpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#include "TrackDAOSqlite3Impl.hpp"

TrackDAOSqlite3Impl::TrackDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO, AlbumDAO* albumDAO ) {
	this->db = db;
	this->artistDAO = artistDAO;
	this->albumDAO = albumDAO;
}

TrackDAOSqlite3Impl::~TrackDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

Track* TrackDAOSqlite3Impl::getTrackById( long id ) {
	Track* track;
	int column = 0;

	checkDb();

	sqlite3_stmt* pStmt = prepare( "SELECT "
			"`Filename`, "
			"`Checksum`, "
			"`Name`, "
			"`Art_Filename`, "
			"`ArtistId`, "
			"`AlbumId`, "
			"`TrackNumber`, "
			"`DiscNumber`, "
			"`ReplayGain`"
			" FROM `Tracks` WHERE `TrackId` = :trackid" );
	bindLong( pStmt, ":trackid", id );
	if( step( pStmt ) != SQLITE_ROW ) {
		return NULL;
	}

	track = new Track();
	track->id = id;
	track->filename.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
	column++; //track->checksum.assign( (const char*)sqlite3_column_text( pStmt, column++;  ) );
	track->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
	track->artFilename.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
	track->artist = artistDAO->getArtistById( sqlite3_column_int64( pStmt, column++ ) );
	track->album = albumDAO->getAlbumById( sqlite3_column_int64( pStmt, column++ ) );
	track->trackNumber = sqlite3_column_int( pStmt, column++ );
	track->discNumber = sqlite3_column_int( pStmt, column++ );
	track->replayGain = sqlite3_column_double( pStmt, column++ );

	finalize( pStmt );

	return track;
}

void TrackDAOSqlite3Impl::insertOrUpdateTrack( Track* track ) {
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

void TrackDAOSqlite3Impl::free( Track* track ) {
	if( track->album != NULL ) {
		albumDAO->free( track->album );
		track->album = NULL;
	}
	if( track->artist != NULL ) {
		artistDAO->free( track->artist );
		track->artist = NULL;
	}
	delete track;
}

void TrackDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	checkDb();

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
		throw new Sqlite3Exception( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

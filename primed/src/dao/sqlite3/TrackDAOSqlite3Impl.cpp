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

	sqlite3_stmt* pStmt = prepare(
			"SELECT "
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
	track->filename.assign( (const char*)sqlite3_column_text( pStmt, column++ ) );
	column++; //track->checksum.assign( (const char*)sqlite3_column_text( pStmt, column++;  ) );
	track->name.assign( (const char*)sqlite3_column_text( pStmt, column++ ) );
	track->artFilename.assign( (const char*)sqlite3_column_text( pStmt, column++ ) );
	column++; //track->artist = artistDAO->getArtistById( sqlite3_column_int64( pStmt, column++ ) )
	column++; //track->album = albumDAO->getAlbumById( sqlite3_column_int64( pStmt, column++ ) )
	track->trackNumber = sqlite3_column_int( pStmt, column++ );
	track->discNumber = sqlite3_column_int( pStmt, column++ );
	track->replayGain = sqlite3_column_double( pStmt, column++ );

	return track;
}

void TrackDAOSqlite3Impl::insertOrUpdateTrack( Track* track ) {
}

void TrackDAOSqlite3Impl::free( Track* track ) {
	if( track->album != NULL ) {
		delete track->album;
		track->album = NULL;
	}
	if( track->artist != NULL ) {
		delete track->artist;
		track->artist = NULL;
	}
	delete track;
}

void TrackDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	if( db == NULL ) {
		throw new Sqlite3Exception( "DB not initialized" );
	}

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

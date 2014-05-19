/*
 * TrackDAOSqlite3Impl.cpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#include <glib.h>
#include "TrackDAOSqlite3Impl.hpp"
#include "GeminiException.hpp"

TrackDAOSqlite3Impl::TrackDAOSqlite3Impl( sqlite3* db, ArtistDAO* artistDAO, AlbumDAO* albumDAO ):
	BaseSqlite3Impl( db ), artistDAO( artistDAO ), albumDAO( albumDAO ) { }

TrackDAOSqlite3Impl::~TrackDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

Track* TrackDAOSqlite3Impl::getTrack( Track* criterion ) {
	Track* track = NULL;
	Artist* artist_criterion;
	Album* album_criterion;
	int column = 0;
	QueryCriteriaList queryCriteriaList;
	std::string* sql;

	checkDb();

	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			QueryCriteria qc = {"TrackId", ":trackid", QueryCriteria::LONG};
			qc.value = &criterion->id;
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->filename.empty() ) {
			QueryCriteria qc = {"Filename", ":filename", QueryCriteria::TEXT};
			qc.value = criterion->filename.c_str();
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->name.empty() ) {
			QueryCriteria qc = {"Name", ":name", QueryCriteria::TEXT};
			qc.value = criterion->name.c_str();
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->artFilename.empty() ) {
			QueryCriteria qc = {"Art_Filename", ":artfilename", QueryCriteria::TEXT};
			qc.value = criterion->artFilename.c_str();
			queryCriteriaList.push_back(qc);
		}
		if( criterion->artist && criterion->artist->id >= 0 ) {
			QueryCriteria qc = {"ArtistId", ":artistid", QueryCriteria::LONG};
			qc.value = &criterion->artist->id;
			queryCriteriaList.push_back(qc);
		}
		if( criterion->album && criterion->album->artist && criterion->album->artist->id >= 0 ) {
			QueryCriteria qc = {"AlbumArtistId", ":albumartistid", QueryCriteria::LONG};
			qc.value = &criterion->album->artist->id;
			queryCriteriaList.push_back(qc);
		}
		if( criterion->album && criterion->album->id >= 0 ) {
			QueryCriteria qc = {"AlbumId", ":albumid", QueryCriteria::LONG};
			qc.value = &criterion->album->id;
			queryCriteriaList.push_back(qc);
		}
		if( criterion->discNumber > 0 ) {
			QueryCriteria qc = {"DiscNumber", ":discnumber", QueryCriteria::INT};
			qc.value = &criterion->discNumber;
			queryCriteriaList.push_back(qc);
		}
		if( criterion->trackNumber > 0 ) {
			QueryCriteria qc = {"TrackNumber", ":tracknumber", QueryCriteria::INT};
			qc.value = &criterion->trackNumber;
			queryCriteriaList.push_back(qc);
		}
		if( criterion->replayGain > -99.0 ) {
			QueryCriteria qc = {"ReplayGain", ":replaygain", QueryCriteria::DOUBLE};
			qc.value = &criterion->replayGain;
			queryCriteriaList.push_back(qc);
		}
	}

	sql = buildSqlFromQueryCriteria( "Tracks", queryCriteriaList );
	sqlite3_stmt* pStmt = prepare( sql->c_str() );
	bindVariablesFromQueryCriteria( pStmt, queryCriteriaList );

	if( step( pStmt ) == SQLITE_ROW ) {
		track = new Track();
		artist_criterion = new Artist();
		album_criterion = new Album();
		album_criterion->artist = new Artist();

		track->id = sqlite3_column_int64( pStmt, column++ );
		track->filename.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
		column++; //track->checksum.assign( (const char*)sqlite3_column_text( pStmt, column++;  ) );
		track->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
		track->artFilename.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );
		artist_criterion->id = sqlite3_column_int64( pStmt, column++ );
		album_criterion->artist->id = sqlite3_column_int64( pStmt, column++ );
		album_criterion->id = sqlite3_column_int64( pStmt, column++ );
		track->trackNumber = sqlite3_column_int( pStmt, column++ );
		track->discNumber = sqlite3_column_int( pStmt, column++ );
		track->replayGain = sqlite3_column_double( pStmt, column++ );

		if( artist_criterion->id >= 0 ) {
			track->artist = artistDAO->getArtist( artist_criterion );
		}
		if( album_criterion->id >= 0 ) {
			track->album = albumDAO->getAlbum( album_criterion );
		}
		delete album_criterion->artist;
		delete album_criterion;
		delete artist_criterion;
	}

	finalize( pStmt );
	delete sql;

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
				"WHERE `Filename` = :filename;" );
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
	sql.append( "  `ArtistId` INTEGER DEFAULT NULL," );
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
		THROW_GEMINI_EXCEPTION( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

void TrackDAOSqlite3Impl::free( std::vector<long>* track_ids ) {
	delete track_ids;
}

std::vector<long>* TrackDAOSqlite3Impl::getTrackIds( ) {
	std::vector<long>* track_ids;

	checkDb();

	track_ids = new std::vector<long>();

	sqlite3_stmt* pStmt = prepare( "SELECT "
			"`TrackId` "
			" FROM `Tracks`;" );
	while( step( pStmt ) == SQLITE_ROW ) {
		track_ids->push_back( sqlite3_column_int64( pStmt, 0) );
	}

	finalize( pStmt );

	return track_ids;
}

std::vector<long>* TrackDAOSqlite3Impl::getTrackIdsByArtistId( long artist_id ) {
	std::vector<long>* track_ids;

	checkDb();

	track_ids = new std::vector<long>();

	sqlite3_stmt* pStmt = prepare( "SELECT "
			"`TrackId` "
			" FROM `Tracks`"
			" WHERE `ArtistId` = :artistid;");
	bindLong( pStmt, ":artistid", artist_id );
	while( step( pStmt ) == SQLITE_ROW ) {
		track_ids->push_back( sqlite3_column_int64( pStmt, 0) );
	}

	finalize( pStmt );

	return track_ids;
}

std::vector<long>* TrackDAOSqlite3Impl::getTrackIdsByAlbumId( long album_id ) {
	std::vector<long>* track_ids;

	checkDb();

	track_ids = new std::vector<long>();

	sqlite3_stmt* pStmt = prepare( "SELECT "
			"`TrackId`, `DiscNumber`, `TrackNumber` "
			" FROM `Tracks`"
			" WHERE `AlbumId` = :albumid"
			" ORDER BY `DiscNumber` ASC, `TrackNumber` ASC");
	bindLong( pStmt, ":albumid", album_id );
	while( step( pStmt ) == SQLITE_ROW ) {
		track_ids->push_back( sqlite3_column_int64( pStmt, 0) );
	}

	finalize( pStmt );

	return track_ids;
}

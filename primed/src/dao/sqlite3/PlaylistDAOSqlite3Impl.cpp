/*
 * PlaylistDAOSqlite3Impl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <string>
#include <sstream>
#include "PlaylistDAOSqlite3Impl.hpp"
#include "GeminiException.hpp"

PlaylistDAOSqlite3Impl::~PlaylistDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

void PlaylistDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	std::string sql;

	checkDb();

	sql.append( "CREATE TABLE IF NOT EXISTS `Playlists` (" );
	sql.append( "  `PlaylistId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `Name` VARCHAR(255) NOT NULL" );
	sql.append( ");" );
	sql.append( "CREATE TABLE IF NOT EXISTS `PlaylistTracks` (" );
	sql.append( "  `PlaylistTracksId` INTEGER PRIMARY KEY AUTOINCREMENT," );
	sql.append( "  `PlaylistId` INTEGER NOT NULL," );
	sql.append( "  `Order` INTEGER NOT NULL," );
	sql.append( "  `TrackId` INTEGER NOT NULL," );
	sql.append( "  FOREIGN KEY(`TrackId`) REFERENCES `Tracks`(`TrackId`) ON DELETE RESTRICT," );
	sql.append( "  FOREIGN KEY(`PlaylistId`) REFERENCES `Playlists`(`PlaylistId`) ON DELETE RESTRICT" );
	sql.append( ");" );

	rc = sqlite3_exec( db, sql.c_str(), NULL, 0, &zErrMsg );
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

void PlaylistDAOSqlite3Impl::free( Playlist* playlist ) {
	delete playlist;
}

Playlist* PlaylistDAOSqlite3Impl::getPlaylist( Playlist* criterion ) {
	Playlist* playlist;
	int column = 0;
	QueryCriteriaList queryCriteriaList;
	std::string* sql;

	checkDb();

	if( criterion != NULL ) {
		if( criterion->id >= 0 ) {
			QueryCriteria qc = {"PlaylistId", ":playlistid", QueryCriteria::LONG};
			qc.value = &criterion->id;
			queryCriteriaList.push_back(qc);
		}
		if( !criterion->name.empty() ) {
			QueryCriteria qc = {"Name", ":name", QueryCriteria::TEXT};
			qc.value = criterion->name.c_str();
			queryCriteriaList.push_back(qc);
		}
	}

	sql = buildSqlFromQueryCriteria( "Playlists", queryCriteriaList );
	sqlite3_stmt* pStmt = prepare( sql->c_str() );
	bindVariablesFromQueryCriteria( pStmt, queryCriteriaList );

	if( step( pStmt ) != SQLITE_ROW ) {
		return NULL;
	}

	playlist = new Playlist();
	playlist->id = sqlite3_column_int64( pStmt, column++ );
	playlist->name.assign( (const char*) sqlite3_column_text( pStmt, column++ ) );

	finalize( pStmt );
	delete sql;

	fillPlaylistTrackIds( playlist );

	return playlist;
}

void PlaylistDAOSqlite3Impl::insertOrUpdatePlaylist( Playlist* playlist ) {
	sqlite3_stmt* pStmt;

	playlist->id = -1;

	pStmt = prepare( "SELECT `PlaylistId` from `Playlists` WHERE `Name` = :name;" );
	bindText( pStmt, ":name", playlist->name.c_str() );
	if( step( pStmt ) == SQLITE_ROW ) {
		playlist->id = sqlite3_column_int64( pStmt, 0 );
	}
	finalize( pStmt );

	if( playlist->id < 0 ) {
		pStmt = prepare( "INSERT INTO `Playlists` (`Name`) VALUES (:name);");
		bindText( pStmt, ":name", playlist->name.c_str() );
		step( pStmt );
		finalize( pStmt );
		playlist->id = sqlite3_last_insert_rowid( db );
	} else {
		pStmt = prepare( "UPDATE `Playlists` SET `name` = :name WHERE `PlaylistId` = :playlistid;" );
		bindText( pStmt, ":name", playlist->name.c_str() );
		bindLong( pStmt, ":playlistid", playlist->id );
		step( pStmt );
		finalize( pStmt );
	}

	pStmt = prepare( "DELETE FROM `PlaylistTracks` where `PlaylistId` = :playlistid;" );
	bindLong( pStmt, ":playlistid", playlist->id );
	step( pStmt );
	finalize( pStmt );

	std::stringstream sql;

	char separator = ' ';
	for( Playlist::TrackIdsIterator iter_track_ids = playlist->trackIds.begin(); iter_track_ids != playlist->trackIds.end(); iter_track_ids++ ) {
		long& track_id = *iter_track_ids;
		long order = iter_track_ids - playlist->trackIds.begin();
		if( order % 500 == 0 ) {
			if( order > 0 ) {
				sql << ";";
				pStmt = prepare( sql.str().c_str() );
				step( pStmt );
				finalize( pStmt );
				sql.str(std::string());
			}
			sql << "INSERT INTO `PlaylistTracks` (`PlaylistId`, `Order`, `TrackId`) VALUES";
			separator = ' ';
		}
		sql << separator << "(" << playlist->id << "," << order << "," << track_id << ")";
		separator = ',';
	}
	sql << ";";
	pStmt = prepare( sql.str().c_str() );
	step( pStmt );
	finalize( pStmt );
}

void PlaylistDAOSqlite3Impl::fillPlaylistTrackIds( Playlist* playlist ) {
	std::stringstream sql;

	checkDb();

	sql << "SELECT TrackId FROM `PlaylistTracks`";
	sql << " WHERE PlaylistId = :playlistid";
	sql << " ORDER BY `Order` ASC;";
	sqlite3_stmt* pStmt = prepare( sql.str().c_str() );
	bindLong( pStmt, ":playlistid", playlist->id );

	while( step( pStmt) == SQLITE_ROW ) {
		playlist->trackIds.push_back( sqlite3_column_int64( pStmt, 0 ) );
	}

	finalize( pStmt );
}

void PlaylistDAOSqlite3Impl::deletePlaylist( Playlist* playlist ) {
	sqlite3_stmt* pStmt;

	checkDb();

	pStmt = prepare( "DELETE FROM `PlaylistTracks` WHERE `PlaylistId` = :playlistid;" );
	bindLong( pStmt, ":playlistid", playlist->id );
	step( pStmt );
	finalize( pStmt );

	pStmt = prepare( "DELETE FROM `Playlists` WHERE `PlaylistId` = :playlistid;" );
	bindLong( pStmt, ":playlistid", playlist->id );
	step( pStmt );
	finalize( pStmt );
}

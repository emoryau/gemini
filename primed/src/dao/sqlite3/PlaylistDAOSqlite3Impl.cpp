/*
 * PlaylistDAOSqlite3Impl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: emory.au
 */

#include "PlaylistDAOSqlite3Impl.hpp"

#include <string>
#include <sstream>

PlaylistDAOSqlite3Impl::~PlaylistDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

PlaylistDAOSqlite3Impl::PlaylistDAOSqlite3Impl( sqlite3* db ) {
	this->db = db;
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
		throw new Sqlite3Exception( zErrMsg );
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
}

void PlaylistDAOSqlite3Impl::fillPlaylistTrackIds( Playlist* playlist ) {
	std::stringstream sql;

	checkDb();

	sql << "SELECT TrackId FROM `PlaylistTracks`";
	sql << " WHERE PlaylistId = :playlistid";
	sql << " ORDER BY `Order`;";
	sqlite3_stmt* pStmt = prepare( sql.str().c_str() );
	bindLong( pStmt, ":playlistid", playlist->id );

	while( step( pStmt) == SQLITE_ROW ) {
		playlist->trackIds.push_back( sqlite3_column_int64( pStmt, 0 ) );
	}

	finalize( pStmt );
}

/*
 * BaseSqlite3Impl.cpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#include "BaseSqlite3Impl.hpp"


void BaseSqlite3Impl::checkDb() {
	if( db == NULL ) {
		throw new Sqlite3Exception( "DB not initialized" );
	}}

sqlite3_stmt* BaseSqlite3Impl::prepare( const char* sql ) {
	int rc;
	sqlite3_stmt *pStmt = NULL;

	rc = sqlite3_prepare_v2( db, sql, -1, &pStmt, NULL);
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( sqlite3_errmsg( db ) );
	}
	return pStmt;
}

void BaseSqlite3Impl::bindLong( sqlite3_stmt* pStmt, const char* field, const long l ) {
	int rc;
	rc = sqlite3_bind_int64( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			l);
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( sqlite3_errstr(rc) );
	}
}

void BaseSqlite3Impl::bindDouble( sqlite3_stmt* pStmt, const char* field, const double d ) {
	int rc;
	rc = sqlite3_bind_double( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			d);
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( sqlite3_errstr(rc) );
	}
}

void BaseSqlite3Impl::bindText( sqlite3_stmt* pStmt, const char* field, const char* text ) {
	int rc;
	rc = sqlite3_bind_text( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			text,
			-1,
			SQLITE_STATIC );
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( sqlite3_errstr(rc) );
	}
}

int BaseSqlite3Impl::step( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_step( pStmt );
	if( rc != SQLITE_DONE && rc != SQLITE_ROW ) {
		throw new Sqlite3Exception( sqlite3_errmsg( db ) );
	}
	return rc;
}

void BaseSqlite3Impl::finalize( sqlite3_stmt* ppStmt ) {
	int rc;
	rc = sqlite3_finalize( ppStmt );
	if( rc != SQLITE_OK ) {
		throw new Sqlite3Exception( sqlite3_errmsg( db ) );
	}
}

/*
 * BaseSqlite3Impl.cpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#include "BaseSqlite3Impl.hpp"
#include "GeminiException.hpp"
#include <sstream>


void BaseSqlite3Impl::checkDb() {
	if( db == NULL ) {
		THROW_GEMINI_EXCEPTION( "DB not initialized" );
	}}

sqlite3_stmt* BaseSqlite3Impl::prepare( const char* sql ) {
	int rc;
	sqlite3_stmt *pStmt = NULL;

	rc = sqlite3_prepare_v2( db, sql, -1, &pStmt, NULL);
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errmsg( db ) );
	}
	return pStmt;
}

void BaseSqlite3Impl::bindInt( sqlite3_stmt* pStmt, const char* field, int i ) {
	int rc;
	rc = sqlite3_bind_int( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			i);
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errstr(rc) );
	}
}

void BaseSqlite3Impl::bindLong( sqlite3_stmt* pStmt, const char* field, const long l ) {
	int rc;
	rc = sqlite3_bind_int64( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			l);
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errstr(rc) );
	}
}

void BaseSqlite3Impl::bindDouble( sqlite3_stmt* pStmt, const char* field, const double d ) {
	int rc;
	rc = sqlite3_bind_double( pStmt,
			sqlite3_bind_parameter_index( pStmt, field ),
			d);
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errstr(rc) );
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
		THROW_GEMINI_EXCEPTION( sqlite3_errstr(rc) );
	}
}

int BaseSqlite3Impl::step( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_step( pStmt );
	if( rc != SQLITE_DONE && rc != SQLITE_ROW ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errmsg( db ) );
	}
	return rc;
}

void BaseSqlite3Impl::finalize( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_finalize( pStmt );
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errmsg( db ) );
	}
}


std::string* BaseSqlite3Impl::buildSqlFromQueryCriteria( const char* table_name, QueryCriteriaList& queryCriteriaList ) {
	std::stringstream sql;
	bool included_where = false;

	sql << "SELECT * FROM `" << table_name << "`";
	for( QueryCriteriaList::iterator qc_iter = queryCriteriaList.begin(); qc_iter != queryCriteriaList.end(); qc_iter++ ) {
		QueryCriteria& qc = *qc_iter;
		if( included_where ) {
			sql << " AND ";
		} else {
			sql << " WHERE ";
			included_where = true;
		}
		sql << "`" << qc.field_name << "` = " << qc.bind_var;
	}
	sql << ';';

	return new std::string( sql.str() );
}

void BaseSqlite3Impl::bindVariablesFromQueryCriteria( sqlite3_stmt* pStmt, QueryCriteriaList& queryCriteriaList ) {
	for( QueryCriteriaList::iterator qc_iter = queryCriteriaList.begin(); qc_iter != queryCriteriaList.end(); qc_iter++ ) {
		QueryCriteria& qc = *qc_iter;
		switch( qc.field_type ) {
			case QueryCriteria::TEXT:
				bindText( pStmt, qc.bind_var, (const char*)qc.value );
				break;
			case QueryCriteria::INT:
				bindInt( pStmt, qc.bind_var, *(int*)qc.value );
				break;
			case QueryCriteria::LONG:
				bindLong( pStmt, qc.bind_var, *(const long*)qc.value );
				break;
			case QueryCriteria::DOUBLE:
				bindDouble( pStmt, qc.bind_var, *(const double*)qc.value );
				break;
		}
	}
}

void BaseSqlite3Impl::reset( sqlite3_stmt* pStmt ) {
	int rc;
	rc = sqlite3_reset( pStmt );
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( sqlite3_errmsg( db ) );
	}
}

/*
 * SettingsDAOSqlite3Impl.cpp
 *
 *  Created on: May 15, 2014
 *      Author: emory.au
 */

#include <glib.h>
#include <stdlib.h>
#include <sstream>
#include "SettingsDAOSqlite3Impl.hpp"
#include "GeminiException.hpp"

SettingsDAOSqlite3Impl::~SettingsDAOSqlite3Impl() {
	// TODO Auto-generated destructor stub
}

void SettingsDAOSqlite3Impl::ensureDBSchema() {
	int rc;
	char *zErrMsg = NULL;
	const char* sql =
			"CREATE TABLE IF NOT EXISTS `Settings` ("
			"`Id` INTEGER PRIMARY KEY AUTOINCREMENT,"
			"`Key` VARCHAR(255) UNIQUE NOT NULL,"
			"`Value` VARCHAR(255)"
			");" ;

	checkDb();

	rc = sqlite3_exec( db, sql, NULL, 0, &zErrMsg );
	if( rc != SQLITE_OK ) {
		THROW_GEMINI_EXCEPTION( zErrMsg );
		sqlite3_free( zErrMsg );
	}
}

void SettingsDAOSqlite3Impl::get( const std::string& key, std::string& value ) {
	const char* sql =
			"SELECT `Value` FROM `Settings`"
			" WHERE `Key` = :key;";
	sqlite3_stmt* pStmt = prepare( sql );
	bindText( pStmt, ":key", key.c_str() );
	if( step( pStmt ) == SQLITE_ROW ) {
		value.assign( (const char*) sqlite3_column_text( pStmt, 0 ) );
	}
	finalize( pStmt );
}

void SettingsDAOSqlite3Impl::get( const std::string& key, long & value ) {
	static std::string s_value;
	get( key, s_value );
	value = atol( s_value.c_str() );
}

void SettingsDAOSqlite3Impl::get( const std::string& key, double& value ) {
	static std::string s_value;
	get( key, s_value );
	value = atof( s_value.c_str() );
}

void SettingsDAOSqlite3Impl::set( const std::string& key, const std::string& value ) {
	const char* sql =
			"INSERT OR REPLACE INTO`Settings`"
			" (`Key`, `Value`) "
			" VALUES(:key, :value);";
	sqlite3_stmt* pStmt = prepare( sql );
	bindText( pStmt, ":key", key.c_str() );
	bindText( pStmt, ":value", value.c_str() );
	step( pStmt );
	finalize( pStmt );
}

void SettingsDAOSqlite3Impl::set( const std::string& key, const long & value ) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << value;
	set( key, ss.str() );
}

void SettingsDAOSqlite3Impl::set( const std::string& key, const double& value ) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << value;
	set( key, ss.str() );
}

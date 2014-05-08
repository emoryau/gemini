/*
 * BaseSqlite3Impl.hpp
 *
 *  Created on: May 7, 2014
 *      Author: emoryau
 */

#ifndef BASESQLITE3IMPL_HPP_
#define BASESQLITE3IMPL_HPP_

#include <string>
#include <sqlite3.h>

class BaseSqlite3Impl {
protected:
	void checkDb();

	sqlite3_stmt* prepare( const char* sql );
	void bindLong( sqlite3_stmt* ppStmt, const char* field, const long l );
	void bindDouble( sqlite3_stmt* ppStmt, const char* field, const double d );
	void bindText( sqlite3_stmt* ppStmt, const char* field, const char* text );
	int step( sqlite3_stmt* pStmt );
	void finalize( sqlite3_stmt* ppStmt );

	sqlite3* db;
};


class Sqlite3Exception: public std::exception {
public:
	Sqlite3Exception( const char* what ) throw() { cause.assign( what );};
	virtual ~Sqlite3Exception() throw() { };

	virtual const char* what() const throw() { return cause.c_str(); };

private:
	std::string cause;
};


#endif /* BASESQLITE3IMPL_HPP_ */

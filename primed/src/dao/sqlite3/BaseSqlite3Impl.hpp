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
#include <list>

class BaseSqlite3Impl {
protected:
	void checkDb();

	sqlite3_stmt* prepare( const char* sql );
	void bindInt( sqlite3_stmt* pStmt, const char* field, int i );
	void bindLong( sqlite3_stmt* pStmt, const char* field, const long l );
	void bindDouble( sqlite3_stmt* pStmt, const char* field, const double d );
	void bindText( sqlite3_stmt* pStmt, const char* field, const char* text );
	int step( sqlite3_stmt* pStmt );
	void reset( sqlite3_stmt* pStmt );
	void finalize( sqlite3_stmt* pStmt );

	sqlite3* db;

	struct QueryCriteria {
		const char* field_name;
		const char* bind_var;
		enum {
			TEXT,
			INT,
			LONG,
			DOUBLE
		} field_type;
		const void* value;
	};
	typedef std::list<QueryCriteria> QueryCriteriaList;
	std::string* buildSqlFromQueryCriteria( const char* table_name, QueryCriteriaList& queryCriteriaList );
	void bindVariablesFromQueryCriteria( sqlite3_stmt* pStmt, QueryCriteriaList& queryCriteriaList );
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

/*
 * MetadataStore.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#ifndef METADATASTORE_HPP_
#define METADATASTORE_HPP_

#include <exception>
#include <sqlite3.h>

#include "Track.hpp"
#include "Artist.hpp"
#include "Album.hpp"

class MetadataStore {
public:
	MetadataStore();
	virtual ~MetadataStore();

	void open( const char* db_filename );
	void addTrack( Track* track );
	long addArtist( Artist* artist );
	long addAlbum( Album* album );
	void addExtractedTrack( TagExtractor& te );

private:
	sqlite3* db;

	void checkDb();
	void ensureDBSchema();
	sqlite3_stmt* prepare( const char* sql );
	void bindLong( sqlite3_stmt* ppStmt, const char* field, const long l );
	void bindDouble( sqlite3_stmt* ppStmt, const char* field, const double d );
	void bindText( sqlite3_stmt* ppStmt, const char* field, const char* text );
	int step( sqlite3_stmt* pStmt );
	void finalize( sqlite3_stmt* ppStmt );
};

class MetadataStoreException: public std::exception {
public:
	MetadataStoreException( const char* what ) throw();
	virtual ~MetadataStoreException() throw() { };

	virtual const char* what() const throw();

private:
	std::string cause;
};

#endif /* METADATASTORE_HPP_ */

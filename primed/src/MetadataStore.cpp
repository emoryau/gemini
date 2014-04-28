/*
 * MetadataStore.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#include <stdio.h>
#include "MetadataStore.hpp"

MetadataStore::MetadataStore() {
	db = NULL;
}

void MetadataStore::Open( const char* db_filename ) {
	int rc;

	rc = sqlite3_open(db_filename, &db);
	if( rc != SQLITE_OK ) {
		sqlite3_close(db);
		// Throw DB exception
		throw "Error opening db";
	}

}
MetadataStore::~MetadataStore() {
	if(db != NULL ) {
		sqlite3_close(db);
		sqlite3_free(db);
		db = NULL;
	}
}


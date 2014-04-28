/*
 * MetadataStore.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#ifndef METADATASTORE_HPP_
#define METADATASTORE_HPP_

#include <sqlite3.h>

class MetadataStore {
public:
	MetadataStore();
	virtual ~MetadataStore();

	void Open( const char* db_filename );

private:
	sqlite3* db;

};

#endif /* METADATASTORE_HPP_ */

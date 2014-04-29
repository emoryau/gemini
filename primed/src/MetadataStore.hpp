/*
 * MetadataStore.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#ifndef METADATASTORE_HPP_
#define METADATASTORE_HPP_

#include <sqlite3.h>

#include "Track.hpp"
#include "Artist.hpp"
#include "Album.hpp"

class MetadataStore {
public:
	MetadataStore();
	virtual ~MetadataStore();

	void open( const char* db_filename );
	void AddTrack( Track* track );
	void AddArtist( Artist* artist );
	void AddAlbum( Album* album );
	void AddExtractedTrack( TagExtractor& te );

private:
	sqlite3* db;

	void ensureDBSchema();
};

#endif /* METADATASTORE_HPP_ */

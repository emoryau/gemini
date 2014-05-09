/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"
#include "TagExtractor.hpp"
#include "MetadataStore.hpp"
#include "TrackManager.hpp"
#include "dao/sqlite3/DAOFactorySqlite3Impl.hpp"
#include <glib.h>

#include <iostream>
#include <exception>

const char* extension_blacklist[] = { ".jpg", ".cue", ".db", ".m3u", ".ini", ".sfv", ".pdf", ".log", ".txt", ".png", NULL };


bool isInBlacklist( const std::string& subject ) {
	const char** extension_comparator = extension_blacklist;
	while( *extension_comparator != NULL ) {
		if( subject.rfind( *extension_comparator ) != std::string::npos ) {
			return true;
		}
		extension_comparator++;
	}
	return false;
}

void updateMetadataStore( const char* directory, const char* store_filename ) {
	Filesystem directory_crawler( directory );
	TagExtractor extractor;
	MetadataStore store;
	TrackManager trackManager;
	DAOFactorySqlite3Impl* daoFactory;

	g_print( "Scanning %s\n", directory );

	// Create some DAO's to talk to the db
	daoFactory = new DAOFactorySqlite3Impl();
	daoFactory->setDBFile( "/home/emory.au/test.sqlite" );
	store.setDAOFactory( daoFactory );

	for( Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it ) {
		// Parse file
		std::string filename = *it;

		if( isInBlacklist( filename ) )
			continue;

		extractor.readTags( filename.c_str() );
		try {
			store.addExtractedTrack( extractor );
		} catch( MetadataStoreException* e ) {
			g_error( "addExtractedTrack() threw:\n%s", e->what() );
		}
	}
}

int main( int argc, char** argv ) {
	gst_init (&argc, &argv);

	const char* folder = "/home/emoryau/testmusic"; // TODO: better default
	const char* db = "/home/emoryau/test.sqlite";

	if( argc > 1 )
		folder = argv[1];
	if( argc > 2 )
		db = argv[2];

	updateMetadataStore( folder, db );

	try {
		DAOFactorySqlite3Impl* daoFactory = new DAOFactorySqlite3Impl();
		daoFactory->setDBFile( db );
		TrackDAO* trackDAO = daoFactory->getTrackDAO();
		Track* track = trackDAO->getTrackById(120);
		g_print( "%s - '%s' - %d.%d - %s\n",
				track->artist ? track->artist->name.c_str() : "<no artist>",
				track->album ? track->album->name.c_str() : "<no album>",
				track->discNumber, track->trackNumber,
				track->name.c_str() );
		trackDAO->free( track );
		delete daoFactory;
	} catch (std::exception* e) {
		g_error( e->what() );
	}

	g_print( "Finished\n" );

	return 0;
}

void printExtractorContents( const TagExtractor& extractor) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t" << extractor.getTrackGain() << "\n";
}

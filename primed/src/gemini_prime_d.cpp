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
#include <gst/gst.h>

#include <iostream>

const char* extension_blacklist[] = { ".jpg", ".cue", ".db", ".m3u", ".ini", ".sfv", ".pdf", ".log", ".txt", NULL };


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


int main( int argc, char** argv ) {
	gst_init (&argc, &argv);
	
	const char* folder = "/home/emoryau/testmusic"; // TODO: better default
	
	if( argc > 1 )
		folder = argv[1];
		
	Filesystem directory_crawler( folder );
	TagExtractor extractor;
	MetadataStore store;
	TrackManager trackManager;

	g_print( "Scanning %s\n", folder );

	// TODO: Database link
	// Create some DAO's to talk to the db
	try {
		store.open("/home/emoryau/test.sqlite");
	} catch (char* e) {
		g_error("DB opening threw:\n%s", e);
		return 1;
	}

	for( Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it ) {
		// Parse file
		std::string filename = *it;

		if( isInBlacklist( filename ) )
			continue;

		extractor.readTags( filename.c_str() );

		// TODO: insert/update database
	}

	g_print( "Finished\n" );

	return 0;
}

void printExtractorContents( const TagExtractor& extractor) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t" << extractor.getTrackGain() << "\n";
}

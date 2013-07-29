/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"
#include "TagExtractor.hpp"
#include <gst/gst.h>

#include <iostream>

int main( int argc, char** argv ) {
	gst_init (&argc, &argv);
	
	const char* folder = "/home/emoryau/testmusic"; // TODO: better default
	
	if( argc > 1 )
		folder = argv[1];
		
	Filesystem directory_crawler( folder );
	TagExtractor extractor;

	g_print( "Scanning %s\n", folder );

	// TODO: Database link

	for( Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it ) {
		// Parse file
		std::string filename;
		filename += "file://";
		filename += *it;

		extractor.readTags(filename.c_str());

		// TODO: insert/update database
	}

	g_print( "Finished\n" );
	return 0;
}

void printExtractorContents( const TagExtractor& extractor) {
	std::cout << extractor.getArtist() << "\t" << extractor.getAlbum() << "\t" << extractor.getTitle() << "\n";
	std::cout << extractor.getDiscNumber() << "\t" << extractor.getTrackNumber() << "\t" << extractor.getAlbumGain() << "\t" << extractor.getTrackGain() << "\n";
}

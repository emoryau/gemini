/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"

#include <iostream>

int main( int argc, char** argv ) {
	Filesystem directory_crawler( "/home/emoryau/testmusic" );

	std::cout << "Files\n";

	for( Filesystem::iterator it = directory_crawler.begin(); it != directory_crawler.end(); ++it ) {
		std::cout << *it << "\n";
	}

	// Database link

	// Load file
	// Parse file
	// insert/update database
	return 0;
}

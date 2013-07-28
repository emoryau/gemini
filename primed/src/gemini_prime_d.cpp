/*
 * gemini_prime_d.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"
#include <gst/gst.h>

int main( int argc, char** argv ) {
	gst_init (&argc, &argv);

	Filesystem* directory_crawler = new Filesystem( "/home/emoryau" );

	return 0;
}

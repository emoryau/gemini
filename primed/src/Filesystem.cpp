/*
 * Filesystem.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#include "Filesystem.hpp"
#include <string>
#include "string.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <list>
#include <fts.h>
#include <errno.h>

Filesystem::Filesystem( const char* workingDirectory ) {
	addDirectory( workingDirectory );
}

Filesystem::Filesystem() {
}

Filesystem::~Filesystem() {
}

void Filesystem::addDirectory( const char *workingDirectory ) {
	char * paths[2] = { NULL, NULL };

	paths[0] = new char[strlen(workingDirectory) + 1];
	strcpy(paths[0], workingDirectory);

	FTS *tree = fts_open( paths, FTS_NOCHDIR, 0 );
	if( !tree ) {
		perror( "fts_open" );
	}

	FTSENT *node;
	while( (node = fts_read( tree )) ) {
		if( node->fts_level > 0 && node->fts_name[0] == '.' )
			fts_set( tree, node, FTS_SKIP );
		else if( node->fts_info & FTS_F ) {
			filenames.push_back( node->fts_path );
		}
	}
	if( errno ) {
		perror( "fts_read" );
		return;
	}

	if( fts_close( tree ) ) {
		perror( "fts_close" );
		return;
	}
}

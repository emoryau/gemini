/*
 * Filesystem.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

class Filesystem {
private:
	Filesystem();
public:
	Filesystem( const char* workingDirectory );
	virtual ~Filesystem();
private:
	void addDirectory( const char *path );
};

#endif /* FILESYSTEM_HPP_ */

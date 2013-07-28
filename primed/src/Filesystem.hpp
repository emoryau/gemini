/*
 * Filesystem.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: emoryau
 */

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#include <list>
#include <string>

class Filesystem {
private:
	Filesystem();
public:
	typedef std::list<std::string> FilenameList;
	typedef FilenameList::iterator iterator;
	typedef FilenameList::const_iterator const_iterator;
	iterator begin() { return filenames.begin(); };
	iterator end() { return filenames.end(); };

	Filesystem( const char* workingDirectory );
	virtual ~Filesystem();

	void addDirectory( const char *path );

private:
	FilenameList filenames;
};

#endif /* FILESYSTEM_HPP_ */

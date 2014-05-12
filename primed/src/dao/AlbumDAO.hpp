/*
 * AlbumDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef ALBUMDAO_HPP_
#define ALBUMDAO_HPP_

#include "../Album.hpp"

class AlbumDAO {
public:
	virtual ~AlbumDAO() = 0;
	virtual void ensureDBSchema() = 0;

	virtual void free( Album* album ) = 0;
	virtual Album* getAlbum( Album* criterion ) = 0;
	virtual void insertOrUpdateAlbum( Album* album ) = 0;
};

inline AlbumDAO::~AlbumDAO() {
}
;

#endif /* ALBUMDAO_HPP_ */

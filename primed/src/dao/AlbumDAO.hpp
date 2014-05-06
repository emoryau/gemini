/*
 * AlbumDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef ALBUMDAO_HPP_
#define ALBUMDAO_HPP_


#include "../../Album.hpp"

class AlbumDAO {
public:
	virtual ~AlbumDAO() = 0;

	virtual Track* GetArtistById( long id ) = 0;
	virtual void InsertOrUpdateAlbum( Album* track ) = 0;
};



#endif /* ALBUMDAO_HPP_ */

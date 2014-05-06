/*
 * ArtistDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef ARTISTDAO_HPP_
#define ARTISTDAO_HPP_


#include "../../Artist.hpp"

class ArtistDAO {
public:
	virtual ~ArtistDAO() = 0;

	virtual Track* GetArtistById( long id ) = 0;
	virtual void InsertOrUpdateArtist( Artist* track ) = 0;
};


#endif /* ARTISTDAO_HPP_ */

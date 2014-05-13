/*
 * ArtistDAO.hpp
 *
 *  Created on: May 6, 2014
 *      Author: emory.au
 */

#ifndef ARTISTDAO_HPP_
#define ARTISTDAO_HPP_


#include "Artist.hpp"

class ArtistDAO {
public:
	virtual ~ArtistDAO() = 0;
	virtual void ensureDBSchema() = 0;

	virtual void free( Artist* artist ) = 0;
	virtual Artist* getArtist( Artist* criterion ) = 0;
	virtual void insertOrUpdateArtist( Artist* artist ) = 0;
};

inline ArtistDAO::~ArtistDAO() { };

#endif /* ARTISTDAO_HPP_ */

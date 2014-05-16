/*
 * MetadataStore.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#ifndef METADATASTORE_HPP_
#define METADATASTORE_HPP_

#include <exception>

#include "Track.hpp"
#include "Artist.hpp"
#include "Album.hpp"
#include "dao/DAOFactory.hpp"

class MetadataService {
public:
	MetadataService(): daoFactory( NULL ) { };
	MetadataService( DAOFactory* daoFactory );
	virtual ~MetadataService();

	void setDAOFactory( DAOFactory* daoFactory) { this->daoFactory = daoFactory; };
	void addExtractedTrack( TagExtractor& te );

private:
	DAOFactory* daoFactory;
};


#endif /* METADATASTORE_HPP_ */

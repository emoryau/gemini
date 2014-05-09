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

class MetadataStore {
public:
	MetadataStore(): daoFactory( NULL ) { };
	MetadataStore( DAOFactory* daoFactory );
	virtual ~MetadataStore();

	void setDAOFactory( DAOFactory* daoFactory) { this->daoFactory = daoFactory; };
	void addExtractedTrack( TagExtractor& te );

private:
	DAOFactory* daoFactory;
};

class MetadataStoreException: public std::exception {
public:
	MetadataStoreException( const char* what ) throw();
	virtual ~MetadataStoreException() throw() { };

	virtual const char* what() const throw();

private:
	std::string cause;
};

#endif /* METADATASTORE_HPP_ */

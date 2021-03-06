/*
 * MetadataStore.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#ifndef METADATASTORE_HPP_
#define METADATASTORE_HPP_

#include "Track.hpp"
#include "Artist.hpp"
#include "Album.hpp"
#include "TagExtractor.hpp"
#include "dao/DAOFactory.hpp"

class MetadataService {
public:
	MetadataService(): dao_factory( NULL ) { };
	MetadataService( DAOFactory* daoFactory );
	virtual ~MetadataService();

	void setDAOFactory( DAOFactory* daoFactory) { this->dao_factory = daoFactory; };
	void insertOrUpdateExtractedTrack( TagExtractor& te );
	Track* getTrackById( long track_id );
	std::vector<long>* getTrackIdsByArtistId( long artist_id );
	std::vector<long>* getTrackIdsByAlbumId( long artist_id );
	void free( Track* track );
	void free( std::vector<long>* track_ids );

private:
	DAOFactory* dao_factory;
};


#endif /* METADATASTORE_HPP_ */

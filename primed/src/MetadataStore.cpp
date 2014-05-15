/*
 * MetadataStore.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#include "MetadataStore.hpp"
#include <glib.h>

MetadataStore::MetadataStore( DAOFactory* daoFactory ) {
	this->daoFactory = daoFactory;
}

MetadataStore::~MetadataStore() {
}

void MetadataStore::addExtractedTrack( TagExtractor& te ) {
	Track* track = new Track( te );
	Album* album = new Album( te );
	Artist* artist = new Artist( te );

	album->artist = artist;
	track->album = album;
	track->artist = artist;

	daoFactory->getArtistDAO()->insertOrUpdateArtist( artist );
	daoFactory->getAlbumDAO()->insertOrUpdateAlbum( album );
	daoFactory->getTrackDAO()->insertOrUpdateTrack( track );

	delete track;
	delete album;
	delete artist;
}

/*
 * MetadataStore.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: emoryau
 */

#include <glib.h>

#include "MetadataService.hpp"

MetadataService::MetadataService( DAOFactory* dao_factory ): dao_factory( dao_factory ) {
}

MetadataService::~MetadataService() {
}

void MetadataService::insertOrUpdateExtractedTrack( TagExtractor& te ) {
	Track* track = new Track();
	Album* album = new Album();
	Artist* album_artist = new Artist();
	Artist* artist = new Artist();

	track->filename.assign( te.getFile_uri() );
	track->artFilename.assign( "" ); // TODO: Get art somehow
	if( te.getTitle() ) {
		track->name.assign( te.getTitle() );
	}
	track->replayGain = te.getTrackGain();
	track->trackNumber = te.getTrackNumber();
	track->discNumber = te.getDiscNumber();

	if( te.getArtist() != NULL ) {
		artist->name.assign( te.getArtist() );
		track->artist = artist;
	}
	if( te.getAlbum() != NULL ) {
		album->name.assign( te.getAlbum() );
		album->replayGain = te.getAlbumGain();
		track->album = album;
		if( te.getAlbumArtist() != NULL ) {
			album_artist->name.assign( te.getAlbumArtist() );
			track->album->artist = album_artist;
		}
	}

	if( track->artist != NULL ) {
		dao_factory->getArtistDAO()->insertOrUpdateArtist( track->artist );
	}
	if( track->album ) {
		if( track->album->artist != NULL ) {
			dao_factory->getArtistDAO()->insertOrUpdateArtist( track->album->artist );
		}
		dao_factory->getAlbumDAO()->insertOrUpdateAlbum( track->album );
	}
	dao_factory->getTrackDAO()->insertOrUpdateTrack( track );

	delete track;
	delete album_artist;
	delete album;
	delete artist;
}

Track* MetadataService::getTrackById( long track_id ) {
	TrackDAO* track_dao = dao_factory->getTrackDAO();
	Track criterion;
	criterion.id = track_id;
	Track* track = track_dao->getTrack( &criterion );
	return track;
}

void MetadataService::free( Track* track ) {
	dao_factory->getTrackDAO()->free( track );
}

std::vector<long>* MetadataService::getTrackIdsByArtistId( long artist_id ) {
	return dao_factory->getTrackDAO()->getTrackIdsByArtistId( artist_id );
}

std::vector<long>* MetadataService::getTrackIdsByAlbumId( long album_id ) {
	return dao_factory->getTrackDAO()->getTrackIdsByAlbumId( album_id );
}

void MetadataService::free( std::vector<long>* track_ids ) {
	dao_factory->getTrackDAO()->free( track_ids );
}

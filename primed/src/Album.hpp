/*
 * Album.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: emoryau
 */

#ifndef ALBUM_HPP_
#define ALBUM_HPP_
 
#include <string>
#include "Artist.hpp"

class Album {
public:
        bool operator< (const Album &a) const { return name < a.name; };

        std::string name;
        float replayGain;
        const Artist* artist;
};

#endif /* ALBUM_HPP_ */

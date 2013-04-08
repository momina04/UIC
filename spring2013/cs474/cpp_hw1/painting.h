/*
 * =====================================================================================
 *
 *       Filename:  painting.h
 *
 *    Description:  Painting
 *
 *        Version:  1.0
 *        Created:  04/08/2013 09:02:20 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef PAINTING_H
#define PAINTING_H

#include "string.h"
#include "artist.h"

class painting_t{
    private:
        string_t title;
        artist_t &artist;
        unsigned int height,width;
        
    public:
};

#endif

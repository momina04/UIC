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
#include <iostream>

using namespace std;
class artist_t;
class painting_t{
    protected:
        const string_t title;
        const artist_t &artist;
        const int height;
        const int width;

        static int unique_id;
        const int id;

    public:
        /* No default constructor as we have a reference of artist. */
        painting_t(const painting_t &);
        painting_t(const string_t &,
                   const artist_t &artist,
                   const unsigned int, 
                   const unsigned int);

        bool operator<(const painting_t&) const;
        bool operator==(const painting_t&) const;
        bool operator<=(const painting_t&) const;

        friend ostream& operator<<(ostream &, const painting_t &);
};


ostream& operator<<(ostream &, const painting_t &);

#endif

/*
 * =====================================================================================
 *
 *       Filename:  landscape.h
 *
 *    Description:  Landscape derived from painting_t
 *
 *        Version:  1.0
 *        Created:  05/03/2013 05:46:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


#ifndef LANDSCAPE_H
#define LANDSCAPE_H 

#include "painting.h"
#include "artist.h"
#include "string.h"
#include <iostream>

using namespace std;

class landscape_t:public painting_t {
   
    string_t country;
    public:
    landscape_t(string_t title,
                 const artist_t &artist,
                  int height,
                   int width,
                    string_t country_):painting_t(title,artist,height,width), country(country_){
    }

    landscape_t(const landscape_t & landscape):painting_t(landscape), country(landscape.country){
    }

    void display() const
    {
        cout<<"@Landscape@,"<<country;
    }

    landscape_t& vcopy() const
    {
        return *(new landscape_t(*this));
    }
 
};
#endif

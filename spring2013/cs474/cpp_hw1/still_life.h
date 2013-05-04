/*
 * =====================================================================================
 *
 *       Filename:  still_life.h
 *
 *    Description:  Still_life derived from painting_t
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


#ifndef STILL_LIFE_H
#define STILL_LIFE_H 

#include "painting.h"
#include "artist.h"
#include "string.h"
#include <iostream>

using namespace std;

class still_life_t:public painting_t {
   
    string_t color_type;
    public:
    still_life_t(string_t title,
                 const artist_t &artist,
                  int height,
                   int width,
                    int color_type_no):painting_t(title,artist,height,width){

        if(color_type_no==1){
            color_type="Water Colors";
        }
        else{
            color_type="Oil Based Colors";
        }

    }

    still_life_t(const still_life_t & still_life):painting_t(still_life), color_type(still_life.color_type){
    }

    void display() const
    {
        cout<<"@Still Life@,"<<color_type;
    }

    still_life_t& vcopy() const
    {
        return *(new still_life_t(*this));
    }
 
};
#endif

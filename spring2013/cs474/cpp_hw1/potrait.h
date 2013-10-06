/*
 * =====================================================================================
 *
 *       Filename:  potrait.h
 *
 *    Description:  Potrait derived from painting_t
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


#ifndef POTRAIT_H
#define POTRAIT_H 

#include "painting.h"
#include "artist.h"
#include "string.h"
#include "linked_list.h"
#include "linked_list.cpp"
#include <iostream>

using namespace std;

class potrait_t:public painting_t {
   
    linked_list_t <string_t> people;
    public:
    potrait_t(string_t title,
                 const artist_t &artist,
                  int height,
                   int width,
                    linked_list_t <string_t> people_):painting_t(title,artist,height,width),people(people_){
    }

    potrait_t(const potrait_t & potrait):painting_t(potrait), people(potrait.people){
    }

    void display() const
    {
        cout<<"@potrait@,"<<people;
    }

    potrait_t& vcopy() const
    {
        return *(new potrait_t(*this));
    }

    virtual ~potrait_t()
    {
        people.deleteAll();
    }
 
};
#endif

/*
 * =====================================================================================
 *
 *       Filename:  artist.h
 *
 *    Description:  Artist class
 *
 *        Version:  1.0
 *        Created:  04/08/2013 09:04:36 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef ARTIST_H
#define ARTIST_H

#include "linked_list.h"

class artist_t{
    private:
        string_t last_name;
        string_t first_name;
        linked_list_t painting_list;
    public:
};
#endif

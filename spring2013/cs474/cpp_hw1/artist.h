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
#include "linked_list.cpp"

#include "painting.h"

class landscape_t;
class still_life_t;
class painting_t;
class artist_t{
    private:
        const string_t last_name;
        const string_t first_name;
        linked_list_t<painting_t> painting_list;
        unsigned int cnt_paintings;
    public:
        /* No default constructor as we will not allow an uninitialized artist */
        artist_t(const string_t&, const string_t &); 
        artist_t(const artist_t &);
        unsigned int get_cnt_paintings() const; 

        void artist_add_painting(const painting_t &painting);
        void artist_add_painting_landscape(const landscape_t &landscape);
        void artist_add_painting_still_life(const still_life_t &still_life);
        bool artist_remove_painting(const painting_t &painting);
        const painting_t * artist_search_painting(const painting_t &painting) const;
        void delete_all_paintings();

        bool operator <(const artist_t&) const;
        bool operator ==(const artist_t&) const;
        bool operator <=(const artist_t&) const;

        ~artist_t();

        friend ostream& operator<<(ostream &, const artist_t &);
        friend class painting_manager_t;
};

ostream& operator<<(ostream &, const artist_t &);
#endif

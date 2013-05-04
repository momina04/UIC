/*
 * =====================================================================================
 *
 *       Filename:  painting_manager.h
 *
 *    Description:  Header for Painting Manager
 *
 *        Version:  1.0
 *        Created:  04/08/2013 07:01:35 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#ifndef PAINTING_MANAGER
#define PAINTING_MANAGER

#include "string.h"
#include "linked_list.h"
#include "linked_list.cpp"
#include "artist.h"
#include "painting.h"

class painting_manager_t{
    private:
        linked_list_t<artist_t> artist_list;
        unsigned int cnt_artist;
        
        const artist_t* search_artist(const artist_t &artist);
    public:
        painting_manager_t();
//        painting_manager_t(const painting_manager_t &painting_manager);
        unsigned int get_cnt_artist() const;
        bool add_artist(string_t artist_last_name, string_t artist_first_name);
        bool add_painting(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width);
        bool add_painting_landscape(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width, string_t country);
        bool add_painting_still_life(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width, int color_type);
        bool add_painting_potrait(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width, linked_list_t<string_t> &people);
        bool clear_artist(string_t artist_last_name, string_t artist_first_name);
        bool remove_painting(int id);
        bool copy_artist(string_t artist_last_name, string_t artist_first_name, string_t artist_last_name_new, string_t artist_first_name_new);

        ~painting_manager_t();

        friend ostream& operator<<(ostream&, const painting_manager_t&);
};

ostream& operator<<(ostream& cout, const painting_manager_t& painting_manager);
#endif

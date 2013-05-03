/*
 * =====================================================================================
 *
 *       Filename:  painting_manager.cpp
 *
 *    Description:  Implementation of Painting Manager
 *
 *        Version:  1.0
 *        Created:  04/08/2013 07:01:57 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "painting_manager.h"
#include "linked_list.h"
#include "linked_list.cpp"
#include "landscape.h"
#include "still_life.h"

painting_manager_t::painting_manager_t()
{
    cnt_artist=0;
}


/*
painting_manager_t::painting_manager_t(const painting_manager_t &painting_manager):artist_list(painting_manager.artist_list)
{
    cnt_artist = painting_manager.cnt_artist;
}
*/




unsigned int painting_manager_t::get_cnt_artist() const
{
    return cnt_artist;
}

const artist_t* painting_manager_t::search_artist(const artist_t &artist)
{
    unsigned int idx;
    for(idx=0;idx<cnt_artist;idx++){
        if(*(artist_list[idx])==artist)
            return artist_list[idx];
    }
    return NULL;
}

bool painting_manager_t::add_artist(string_t artist_last_name, string_t artist_first_name)
{
    const artist_t artist(artist_last_name, artist_first_name);
    const artist_t *a1=NULL;
    a1=search_artist(artist);
    
    if(a1){
        return false;
    }
    else {
        artist_list.add_first(*(new artist_t(artist)));
        cnt_artist++;
        return true;
    }
};

bool painting_manager_t::add_painting(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width)
{
    const artist_t *a = NULL;
    const artist_t dummy_artist(artist_last_name,artist_first_name);
    a = artist_list.search(dummy_artist);
    if(a){ /* IF artist exists in system */
        (*a).artist_add_painting(painting_t(title,*a,height,width));
        return true;
    }
    else
    { /* IF artist is a new artist */
        a = new artist_t(artist_last_name,artist_first_name);
        artist_list.add_first(*a);
        a->artist_add_painting(painting_t(title,*a,height,width));
        cnt_artist++;
        return true;
    }
    return false;
}

bool painting_manager_t::add_painting_landscape(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width, string_t country)
{
    const artist_t *a = NULL;
    const artist_t dummy_artist(artist_last_name,artist_first_name);
    a = artist_list.search(dummy_artist);
    if(a){ /* IF artist exists in system */
        (*a).artist_add_painting_landscape(landscape_t(title,*a,height,width,country));
        return true;
    }
    else
    { /* IF artist is a new artist */
        a = new artist_t(artist_last_name,artist_first_name);
        artist_list.add_first(*a);
        a->artist_add_painting_landscape(landscape_t(title,*a,height,width,country));
        cnt_artist++;
        return true;
    }
    return false;
}

bool painting_manager_t::add_painting_still_life(string_t title, string_t  artist_last_name, string_t artist_first_name, unsigned int height, unsigned int width, int color_type)
{
    const artist_t *a = NULL;
    const artist_t dummy_artist(artist_last_name,artist_first_name);
    a = artist_list.search(dummy_artist);
    if(a){ /* IF artist exists in system */
        (*a).artist_add_painting_still_life(still_life_t(title,*a,height,width,color_type));
        return true;
    }
    else
    { /* IF artist is a new artist */
        a = new artist_t(artist_last_name,artist_first_name);
        artist_list.add_first(*a);
        a->artist_add_painting_still_life(still_life_t(title,*a,height,width,color_type));
        cnt_artist++;
        return true;
    }
    return false;
}


bool painting_manager_t::clear_artist(string_t artist_last_name, string_t artist_first_name)
{
    const artist_t dummy_artist(artist_last_name, artist_first_name);
    const artist_t *a;
    a=search_artist(dummy_artist);
    if(a) {
        a->delete_all_paintings();
        return true;
    }
    else{
        return false;
    }
}

bool painting_manager_t::remove_painting(int id)
{
    artist_t dummy_artist("","");
    painting_t dummy_painting("",dummy_artist,0,0,id);
    unsigned int idx;
    bool flag_found=false;
    const artist_t *a1=NULL;
    for(idx=0;idx<cnt_artist;idx++){
        a1 = artist_list[idx];
        if(a1 && a1->artist_search_painting(dummy_painting))
        {
            flag_found = true;
            break;
        }
    }
    if(flag_found==false) return false;
    const painting_t *p1 = NULL;
    a1->artist_remove_painting(dummy_painting);
    return true;
}

bool painting_manager_t::copy_artist(string_t artist_last_name, string_t artist_first_name, string_t artist_last_name_new, string_t artist_first_name_new)
{
    if(artist_last_name + artist_first_name == artist_last_name_new + artist_first_name_new) return false;
    const artist_t *a1;
    a1 = search_artist(artist_t(artist_last_name, artist_first_name));
    if(a1== NULL){
        return false;
    }
    else{
        artist_t *a2 = new artist_t(*a1);
        a2->last_name = artist_last_name_new;
        a2->first_name = artist_first_name_new;
        artist_list.add_first(*a2);
        cnt_artist++;
    }
}

painting_manager_t::~painting_manager_t(){
    artist_list.deleteAll();
}

ostream& operator<<(ostream& cout, const painting_manager_t& painting_manager)
{
    cout<<"PaintingManager: "<<painting_manager.cnt_artist<<" artists{ "<<painting_manager.artist_list<<" }."; 
    return cout;
}

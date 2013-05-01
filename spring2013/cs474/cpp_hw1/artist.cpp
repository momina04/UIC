/*
 * =====================================================================================
 *
 *       Filename:  artist.cpp
 *
 *    Description:  Implementation of artist clas
 *
 *        Version:  1.0
 *        Created:  04/08/2013 04:17:41 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "linked_list.h"
#include "linked_list.cpp"
#include "artist.h"
#include "string.h"

artist_t::artist_t(const string_t& last_name, 
                   const string_t& first_name):last_name(last_name),first_name(first_name)
{
    cnt_paintings = 0;
}

artist_t::artist_t(const artist_t &artist):painting_list(artist.painting_list),last_name(last_name),first_name(first_name)
{
    cnt_paintings = artist.get_cnt_paintings();
}

unsigned int artist_t::get_cnt_paintings() const
{
    return cnt_paintings;
} 

void artist_t::add_painting(const painting_t &painting)
{
    painting_t *p = new painting_t(painting);
    painting_list.add_priority(*p);
    cnt_paintings++;
}

bool artist_t::remove_painting(const painting_t &painting)
{
    const painting_t *p = NULL;
    p = search_painting(painting);
    if(p==NULL) return false;
    painting_list.remove(*p);
    delete p;
    cnt_paintings--;
    return true;
}

const painting_t * artist_t::search_painting(const painting_t &painting) const
{
    const painting_t *p = NULL;
    p = painting_list.search(painting);
    return p;
}

bool artist_t::operator <(artist_t& artist)
{
    return (last_name + first_name) < (artist.last_name + artist.first_name);
}

bool artist_t::operator ==(artist_t& artist)
{
    return (last_name + first_name) == (artist.last_name + artist.first_name);
}

#define LESS_EQUAL(a,b) (a<b || a ==b)
bool artist_t::operator <=(artist_t& artist)
{
    return LESS_EQUAL(*this,artist);
}

artist_t::~artist_t()
{
    const painting_t *p;
    unsigned int s;
    s = painting_list.get_size();
    for(unsigned int i=0;i<s;i++){
        p=painting_list[i];
        if(p){
            delete p;
        }
    }
}

ostream& operator<<(ostream &cout, const artist_t &artist)
{
    cout<<"Artist["<<artist.last_name<<","<<artist.first_name<<" >>> "<<artist.cnt_paintings<<" Paintings("<<artist.painting_list<<") ].";
    return cout;
}

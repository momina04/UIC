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

painting_manager_t::painting_manager_t()
{
    cnt_artist=0;
}

/*
painting_manager_t::painting_manager_t(const painting_manager_t &painting_manager):artist_list(painting_manager.artist_list)
{
    cnt_artist = painting_manager.get_cnt_artist();
}
*/

unsigned int painting_manager_t::get_cnt_artist() const
{
    return cnt_artist;
}

bool painting_manager_t::add_painting(string_t &title, string_t & artist_last_name, string_t &artist_first_name, unsigned int height, unsigned int width)
{
    const artist_t *a = NULL;
    const artist_t dummy_artist(artist_last_name,artist_first_name);
    a = artist_list.search(dummy_artist);
    if(a){
        (*a).add_painting(painting_t(title,a,height,width));
        cnt_artist++;
        return true;
    }
    {
    const artist_t *a1 = new artist_t(artist_last_name,artist_first_name);
    artist_list.add(a1);
    a=artist_list.search(a1);
    a->add_painting(painting_t(title,a,height,width));
    }
    return false;

}

bool painting_manager_t::remove_painting(string_t &title)
{
    const painting_t *painting = NULL;
    const artist_t dummy_artist("dummy","dummy");
    painting_t p(title,dummy_artist,0,0);;
    const artist_t *a = NULL;

    for(int i=0;i<cnt_artist;i++){
        a = artist_list[i];
        if(!a) break;
        painting=a->search_painting(p);
        if(painting != NULL) break;
    }

    if(painting)
    {
        a->remove_painting(*painting);
        return true;
    }
    return false;
}

bool painting_manager_t::delete_artist(string_t &artist_last_name,
                                       string_t &artist_first_name)
{
    const artist_t *a = NULL;
    const artist_t dummy_artist(artist_last_name,artist_first_name);
    a = artist_list.search(dummy_artist);
    if(a){
        artist_list.remove(*a);
        cnt_artist--;
        return true;
    }
    return false;
    
}

void painting_manager_t::copy_painting(string_t &title)
{
}

painting_manager_t::~painting_manager_t(){
    const artist_t *a;
    unsigned int s;
    s = artist_list.get_size();
    for(unsigned int i=0;i<s;i++){
        a=artist_list[i];
        if(a){
            delete a;
        }
    }
}

ostream& operator<<(ostream& cout, const painting_manager_t& painting_manager)
{
    cout<<"PaintingManager: "<<painting_manager.cnt_artist<<" artists{ "<<painting_manager.artist_list<<" }."; 
    return cout;
}

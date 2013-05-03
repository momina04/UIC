/*
 * =====================================================================================
 *
 *       Filename:  painting.cpp
 *
 *    Description:  Implementation for painting class
 *
 *        Version:  1.0
 *        Created:  04/08/2013 09:49:20 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "string.h"
#include "painting.h"

int painting_t::unique_id = 77654;


painting_t::painting_t(const painting_t &painting):artist(painting.artist),
                                                    title(painting.title),
                                                     height(painting.height),
                                                      width(painting.width),id(unique_id++)
{
}

painting_t::painting_t(const string_t &title, 
                       const artist_t &artist,
                       const unsigned int height, 
                       const unsigned int width):artist(artist),
                                                  title(title),
                                                   height(height),
                                                    width(width),id(unique_id++)
{
}

painting_t::painting_t(const string_t &title, 
                       const artist_t &artist,
                       const unsigned int height, 
                       const unsigned int width,
                       const int id_):artist(artist),
                                                  title(title),
                                                   height(height),
                                                    width(width),id(id_)
{
}

bool painting_t::operator<(const painting_t& painting) const
{
    return (id < painting.id);
}

bool painting_t::operator==(const painting_t& painting) const
{
    return (id == painting.id);
}

#define LESS_EQUAL(a,b) (a<b || a ==b)
bool painting_t::operator<=(const painting_t& painting) const
{
    return LESS_EQUAL(*this,painting);
}

void painting_t::display() const
{
    /*do nothing in base class */
}

painting_t& painting_t::vcopy() const
{
    return *(new painting_t(*this));
}

ostream& operator<<(ostream &cout, const painting_t &painting)
{
    //cout<<"Painting("<<painting.title<<","<<painting.artist.name_string()<<","<<painting.height<<","<<painting.width<<"). ";
    cout<<"Painting("<<"#"<<painting.id<<","<<painting.title<<","<<painting.height<<","<<painting.width<<",";
    painting.display();
    cout<<"). ";
    return cout;
}

painting_t::~painting_t()
{
}

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

#include "painting.h"

int painting_t::unique_id = 77654;

painting_t::painting_t(const painting_t &painting):artist(painting.artist),
                                                    title(painting.title),
                                                     height(painting.height),
                                                      width(painting.width)
{
}

painting_t::painting_t(const string_t &title, 
                       const artist_t &artist,
                       const unsigned int height, 
                       const unsigned int width):artist(artist),
                                                  title(title),
                                                   height(height),
                                                    width(width)
{
}

bool painting_t::operator<(const painting_t& painting) const
{
    return (title < painting.title);
}

bool painting_t::operator==(const painting_t& painting) const
{
    return (title == painting.title);
}

#define LESS_EQUAL(a,b) (a<b || a ==b)
bool painting_t::operator<=(const painting_t& painting) const
{
    return LESS_EQUAL(*this,painting);
}

ostream& operator<<(ostream &cout, const painting_t &painting)
{
    //cout<<"Painting("<<painting.title<<","<<painting.artist.name_string()<<","<<painting.height<<","<<painting.width<<"). ";
    cout<<"Painting("<<painting.title<<","<<painting.height<<","<<painting.width<<"). ";
    return cout;
}

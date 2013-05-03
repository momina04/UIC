/*
 * =====================================================================================
 *
 *       Filename:  hw2.cpp
 *
 *    Description:  Painting Manager System
 *
 *        Version:  1.0
 *        Created:  05/03/2013 03:16:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


#include <iostream>
#include "painting_manager.h"
#include "string.h"
#include "landscape.h"

using namespace std;

#define WATER_COLORS 1
#define OIL_COLORS 2

int main (int argc, char *argv[])
{
    painting_manager_t system;

    assert(system.add_artist("agarwal","ritesh"));
    assert(!system.add_artist("agarwal","ritesh"));
    assert(system.add_painting("p1","agarwal","ritesh",10,20));
    assert(system.add_painting("p1","agarwal","ritesh",10,20));
    assert(system.add_painting("p2","vohra","anuj",10,20));
    assert(system.clear_artist("agarwal","ritesh"));
    assert(!system.clear_artist("rasania","yash"));
    assert(system.add_painting("p1","agarwal","ritesh",10,20));
    assert(system.add_painting("p1","agarwal","ritesh",10,20));
    assert(system.add_painting_landscape("pL","agarwal","ritesh",20,20,"America"));
    assert(system.add_painting_still_life("pSW","agarwal","ritesh",30,20,WATER_COLORS));
    assert(system.add_painting_still_life("pSO","agarwal","ritesh",40,20,OIL_COLORS));
    assert(system.remove_painting(77659));
//    assert(!system.remove_painting(77659));
//    assert(system.remove_painting(77663));
//    assert(system.remove_painting(77661));
    assert(!system.copy_artist("rasania","yash","dummy","dummy"));
    assert(system.copy_artist("vohra","anuj","rasania","yash"));
    assert(system.copy_artist("agarwal","ritesh","kanchan","abhishek"));
//    assert(system.remove_painting(77666));
    assert(!system.copy_artist("agarwal","ritesh","agarwal","ritesh"));
//    assert(!system.remove_painting(77666));
    assert(system.remove_painting(77672));
    assert(system.remove_painting(77673));
    assert(!system.remove_painting(77672));
    assert(!system.remove_painting(77673));

    cout<<system<<endl;


    return 0;
}/* main */

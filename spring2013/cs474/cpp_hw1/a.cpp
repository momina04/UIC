/*
 * =====================================================================================
 *
 *       Filename:  a.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/07/2013 07:53:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include <cstddef>


int main (int argc, char *argv[])
{
    int j=1,l=2;
    const int &k=j;
    k=l;

    return 0;
}/* main */


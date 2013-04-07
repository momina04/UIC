/*
 * =====================================================================================
 *
 *       Filename:  test_string.cpp
 *
 *    Description:  Test string
 *
 *        Version:  1.0
 *        Created:  04/06/2013 09:00:47 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "string.h"
#include <iostream>
#include <assert.h>

using namespace std;

int main (int argc, char *argv[])
{
    String i("ritesh "),j(i),k="abhishek ",l=k,m;
    cout<<i<<j<<k<<l<<endl;
    i="anuj ";
    cout<<i<<j<<k<<l<<endl;
    j=i;
    cout<<i<<j<<k<<l<<endl;
    cin>>i;
    cout<<i<<endl;
    cout<<m<<endl;
    m="amit ";
    cout<<m<<endl;
    cout<<i + m + i + j + k + l + m + "test1" + "test2"<<endl;
    cout<<i + m + i + j + k + l + m<<endl;

    assert(1 == 1);
//    assert(k == l);
//    l="abhishek";
//    assert(k == l);
//    i=j;
 //   assert(i == j);
    //assert(j != k);


    
    return 0;
}/* main */

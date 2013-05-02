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
    string_t i("ritesh "),j(i),k="abhishek ",l=k,m;
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
    i="random";
    j="seed";
    assert(i!=j || cout<<"i!=j failed"<<endl);
    j=m;
    assert(j==m || cout<<"j==m failed"<<endl);
    k="amit ";
    assert(k=="amit ");
    assert(j==k || cout<<"j==k failed"<<endl);
    assert(i+j != k+j);
    assert(i+i == i+i);
    i=j;
    i+=i;
    j+=j;
    assert(i==j || cout<<"i==j failed"<<endl);
    i="i";
    j="j";
    assert(i<j);
    assert(i<=j);
    assert(j>i);
    assert(j>=i);
    i=j;
    assert(i<=j);
    assert(i>=j);
    assert(!(i>j));
    assert(!(j>i));
    k="k";
    l="kk";
    assert(k<l);
    assert(k<=l);
    assert(l>k);
    assert(l>=k);
    assert(k!=l);
    assert(l!=k);

    string_t s[]={"abc","pqr"};
    cout<<s[0]<<endl;
    cout<<s[1]<<endl;

    string_t *ss=new string_t;
    *ss="alpha";
    cout<<*ss<<endl;
    delete ss;

    string_t *st=new string_t[2];
    st[0]="ll";
    st[1]="pl";
    cout<<st[0]<<endl;
    cout<<st[1]<<endl;
    delete []st;
//    assert(k == l);
//    l="abhishek";
//    assert(k == l);
//    i=j;
 //   assert(i == j);
    //assert(j != k);

/*    
    string_t ijkl("ritesh ");
    char *stuv;
    cout<<ijkl<<endl;
    stuv=(char *)ijkl;
    cout<<stuv<<endl;
    delete[] stuv;
*/
    return 0;
}/* main */

/*
 * =====================================================================================
 *
 *       Filename:  f.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2013 03:22:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


class test
{
    static int *x;

    friend void func1(test &test,int x);
};

int * test::x = new int;


void func1(test &test1,int y)
{
    test::x=&y;
};

int main (int argc, char *argv[])
{
    test o1;

    func1(o1,2);

    return 0;
}/* main */

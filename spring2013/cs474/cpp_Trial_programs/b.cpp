/*
 * =====================================================================================
 *
 *       Filename:  b.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2013 07:59:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


void func1(const char * &x)
{
}


void func1(const char *x)
{
}

void func1(const char x)
{
}

void func1(char *x)
{
}

void func1(char &x)
{
}



int main (int argc, char *argv[])
{
    func1("asd");
    func1('a');
    return 0;
}/* main */

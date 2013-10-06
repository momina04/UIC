/*
 * =====================================================================================
 *
 *       Filename:  d.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2013 10:30:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include <iostream>

using namespace std;

class temp
{
    public:
        temp(int z)
        {
            cout<<"temp - int"<<endl;
        }

        ~temp()
        {
            cout<<"~temp"<<endl;
        }
};

class base{
    public:
        base()
        {
            cout<<"base - default"<<endl;
        }

        base(int x)
        {
            cout<<"base - int x"<<endl;
        }

        ~base()
        {
            cout<<"~base"<<endl;
        }
};

class derived:public base{
        const temp z;
    public:
        derived():z(3),base(2)
        {
            cout<<"derived - default"<<endl;
        }

        derived(int z):z(3)
        {
            cout<<"derived - int"<<endl;
        }

        ~derived()
        {
            cout<<"~derived"<<endl;
        }
};

int main (int argc, char *argv[])
{

    base b;
    cout<<endl;
    derived d;
    cout<<endl;
    derived e(2);

    //e(3);

    return 0;
}/* main */

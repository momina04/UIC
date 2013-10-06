/*
 * =====================================================================================
 *
 *       Filename:  k.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/03/2013 05:28:05 AM
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

class B{
    int x;
    public:
    virtual void display()
    {
        cout<<"base";
    }
    friend ostream& operator<<(ostream &cout, B& b);
};

ostream& operator<<(ostream &cout, B& b)
{
    b.display();
}

class D:public B{
    public:
    virtual void display()
    
    {
        B::display();
        cout<<"derived";

    }
};


int main (int argc, char *argv[])
{
    B &b = *(new D());
    cout<<b;

    delete &b;

    return 0;
}/* main */

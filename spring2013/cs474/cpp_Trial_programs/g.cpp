/*
 * =====================================================================================
 *
 *       Filename:  g.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2013 10:42:27 PM
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

class test
{
    int x;
    public:
        test(){
            cout<<"constructor"<<endl;
            x=0;
        }

        ~test(){
            cout<<"destructor"<<endl;
        }

        void * operator new (size_t size, void *p)
        {
            cout<<"new place"<<endl;
              //return new   char[size];
 //           x=2;
 //           return (void *)1;
//              return new   char[size];
//              return 0;
        }

        void * operator new[] (size_t size, void *p)
        {
            cout<<"new[] place"<<endl;
              return new   char[size];
 //           x=2;
 //           return (void *)1;
//              return new   char[size];
//              return 0;
        }

        void * operator new[] (size_t size)
        {
            cout<<"new[]"<<endl;
              return new   char[size];
 //           x=2;
 //           return (void *)1;
//              return new   char[size];
//              return 0;
        }


        void * operator new(size_t size)
        {
            cout<<"new"<<endl;
 //           x=2;
 //           return (void *)1;
              return new   char[size];
//              return 0;
        }


        void  operator delete[](void *p,void *pool)
        {
           cout<<"delete[] place"<<endl;
           delete[] (char *)p;
        }

        void  operator delete(void *p,void *pool)
        {
           cout<<"delete place"<<endl;
           delete[] (char *)p;
        }

        void  operator delete(void *p)
        {
           cout<<"delete"<<endl;
           delete[] (char *)p;
        }
        void  operator delete[](void *p)
        {
           cout<<"delete[]"<<endl;
           delete[] (char *)p;
        }
};


char pool[sizeof(test)];

int main (int argc, char *argv[])
{
    test e;
    test *s = new test;
    delete s;

    test *s1 = new test[2];
    delete[] s1;

    test *s2 = new (pool) test;
    delete  s2;

    return 0;
}/* main */

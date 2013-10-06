/*
 * =====================================================================================
 *
 *       Filename:  j.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2013 12:14:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */



class B
{
    protected:
        static int x;
        static int y;
};

int B::x=0;
int B::y=0;

class D:public B
{
    public:
        static int x;
        static int f()
        {
            B::x=21;
            x=5;
            y=2;
        }
};

int D::x=2;


int main (int argc, char *argv[])
{
    D::f();
    return 0;
}/* main */

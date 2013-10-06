/*
 * =====================================================================================
 *
 *       Filename:  c.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2013 08:13:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


class testclass
{
    int &x;
    const int y;
    public:
    testclass(int z):x(z),y(z)
    {
    }
};

int main (int argc, char *argv[])
{
    int a=1,b=2;
    int &y=a,&z=a;
    y=b;
    z=y;

    int *p=new int,&q=*p;
    *p=2;
    q=5;
    delete p;
    q=10;



    testclass s(3);


    return 0;
}/* main */

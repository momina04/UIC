/*
 * =====================================================================================
 *
 *       Filename:  h.pp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2013 07:54:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

class D;

class B{
        int x;
    public:
        int f1(int)
        {
        }
        B()
        {
        }

};

class D:public B{
        int y;
    public:
        D()
        {
        }

/*
        D(const B& b)
        {
        }
*/

};

void f2(D d)
{
}

int main (int argc, char *argv[])
{

 //   B b= d.operator B();
    B b;
    D d;
 //   b=d;
//    d=b; //not allowed
    d.f1(2);
    return 0;
}/* main */

/*
 * =====================================================================================
 *
 *       Filename:  e.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2013 01:23:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

class copy_const
{
    public:
        copy_const()
        {
        }
        copy_const(copy_const obj)
        {
        }
};


int main (int argc, char *argv[])
{
    copy_const o1,o2(o1);

    return 0;
}/* main */

/*
 * =====================================================================================
 *
 *       Filename:  random_number_test.c
 *
 *    Description:  Will test how random numbers are generated at different times
 *
 *        Version:  1.0
 *        Created:  11/02/2012 07:36:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include<stdio.h>

//Usage: $0 seed count
int main (int argc, char *argv[])
{
    int seed = atoi(argv[1]);
    int count = atoi(argv[2]);
    int i;
    printf("Seed = %d\n",seed);
    srandom(seed);
    for(i=0; i<count; i++){
        printf("%d. %d\n",i,random());
    }
    return 0;
}/* main */

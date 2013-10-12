/*
 * =====================================================================================
 *
 *       Filename:  gen_input.c
 *
 *    Description:  Generate n random numbers and prints it out on the STDOUT. Format: n 1....n
 *
 *        Version:  1.0
 *        Created:  10/10/2013 11:00:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[]) {


  int n,no,c;
  n=atoi(argv[1]);


  printf("%d\n", n);
 
 
  for (c = 1; c <= n; c++) {
    no = rand()%200 + 1;
    printf("%d ", no);
  }
  printf("\n");
 
  return 0;
}

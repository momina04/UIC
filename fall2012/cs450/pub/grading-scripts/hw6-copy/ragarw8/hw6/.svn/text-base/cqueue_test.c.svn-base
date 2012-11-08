/*
 * =====================================================================================
 *
 *       Filename:  cqueue_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2012 08:40:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */

#include "cqueue.c"

int main(){
    int i,j,k;
    printf("QEmpty = %d\n\n",isQEmpty());
    for(i =0;i<5; i++){
        doQInsert(i);
        printf("i = %d,\n",i);
        printf("QEmpty = %d\n",isQEmpty());
        printf("Q-Full = %d\n",isQFull());
        printf("Q-size = %d\n",getCurrentQSize());
        printf("0th element = %d\n",getNthElement(0));
        //getCurrentQSize();

       // doQDelete();
        //printQ();

    }
    printf("\n");
    for(i =0;i<5; i++){
        j = doQDelete();
        printf("QEmpty = %d\n",isQEmpty());
        printf("Q-Full = %d\n",isQFull());
        printf("Q-size = %d\n",getCurrentQSize());
        printf("0th element = %d\n",getNthElement(0));
        printf("j = %d, i = %d\n",j,i);
    printf("-------\n");
        //getCurrentQSize();

       // doQDelete();
        //printQ();
        //getCurrentQSize();
    }
}

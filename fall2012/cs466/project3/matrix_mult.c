/*
 * =====================================================================================
 *
 *       Filename:  matrix_mult.c
 *
 *    Description:  Program that does matrix multiplication in two ways.
 *
 *        Version:  1.0
 *        Created:  11/24/2012 01:59:52 PM
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
#include <sys/time.h>

unsigned long timeval_to_usec(struct timeval *t) { 
	return t->tv_sec*1000*1000+t->tv_usec;
}

unsigned long current_usec() {
	struct timeval t;
	gettimeofday(&t,0);
	return timeval_to_usec(&t);
}

void multiply_original(int **c, int **a, int **b,int N)
{
    int i,j,k;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++){
            c[i][j] = 0;
            for(k=0;k<N;k++){
                c[i][j] += a[i][k] * b[k][j];
            }
        }
}

void multiply_blocking(int **c, int **a, int **b,int N,int blocking_factor)
{
    int i ,j ,k ,i1 ,j1 ,k1;
    for (i=0; i<N ;i=i+blocking_factor)
        for (j=0; j<N; j=j+blocking_factor){ //Process 1 block of matrix
            for (k=0; k<N ;k=k+blocking_factor)
                /* C_i1,j1 = C_i1,j1 + A_i1,k1 * B_k1,j1 */
                for (i1=i; i1<i+blocking_factor; i1++)
                    for (j1=j; j1<j+blocking_factor; j1++)
                        for (k1=k; k1<k+blocking_factor; k1++)
                            c[i1][j1] = c[i1][j1] + a[i1][k1] * b[k1][j1];
        }
}

int ** allocate_matrix(int N)
{
    int **matrix;
    int *row;
    int i;
    matrix = (int **)calloc(N, sizeof(int *));
    if(matrix == NULL){
        printf("Failed to malloc for size = %d\n",N);
        printf("}\n");
        exit(30);
    }
    for(i=0; i<N; i++){
        row = (int *) calloc(N, sizeof(int));
        if(row == NULL){
        printf("Failed to malloc for size = %d, row =%d\n",N,i);
        printf("}\n");
        exit(32);
        }
        *(matrix + i) =  row;
    }
    return matrix;
}

void fill_matrix(int **m,int N)
{
    int i,j;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            m[i][j] = random()%200; //Value between 0 and 199
}


int compare_matrix(int **c, int **d, int N)
{
    int i,j;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            if(c[i][j] != d [i][j])
                return 1;
    return 0;
}

int main (int argc, char *argv[])
{
    int N;
    int blocking_factor;
    char mode;
    unsigned long start_usec, end_usec;
    int **a, **b, **c;
    
    if(argc < 3 || argc > 4){
        printf("Usage: argv[0] N [o|b] o B - original, b - blocking\n");
        printf("}\n");
        exit(20);
    }

    N = atoi(argv[1]);
    if(argc == 4)
    {
        blocking_factor = atoi(argv[3]);
    }
    mode = argv[2][0];
    if(mode != 'o' && mode != 'b'){
        printf("Usage: argv[0] N [o|b] B  o - original, b - blocking\n");
        printf("}\n");
        exit(20);
    }

    printf("\n{\n");
    printf("Program Name: %s\n", argv[0]);
    printf("Program Mode: %s\n",(mode=='o')?"Original":"Blocking");
    if(mode == 'b'){
        printf("Blocking Factor: %d\n",blocking_factor);
    }
    printf("[ N = %d ]\n", N);


    a = allocate_matrix(N);
    fill_matrix(a,N);
    b = allocate_matrix(N);
    fill_matrix(b,N);
    c = allocate_matrix(N);

    start_usec = current_usec();
    if(mode == 'o'){
        multiply_original(c,a,b,N);
    }
    else if(mode == 'b'){
        multiply_blocking(c,a,b,N,blocking_factor);
    }
    end_usec = current_usec();
    printf("Time taken to multiply = %lu usecs ( %0.3f msecs)\n", end_usec - start_usec , (float)(end_usec - start_usec) / 1000 );
    printf("}\n");


    //#define TEST 1
    #ifdef TEST
    int **d;
    d = allocate_matrix(N);
    if(mode == 'b'){
        multiply_original(d,a,b,N);
    }
    else if(mode == 'o'){
        blocking_factor = 4;
        multiply_blocking(d,a,b,N,blocking_factor);
    }
    if(compare_matrix(c,d,N) != 0){
        printf("Matrix multiplication not same in the two methods.");
        exit(7);
    }
    #endif
    return 0;
}/* main */

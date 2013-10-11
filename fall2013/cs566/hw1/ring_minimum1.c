#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  read_input(int **array_in, int *n)
 *  Description:  Reads in input from STDIN, allocates an array and stores its address in array_in and the count in n.
 * =====================================================================================
 */
void read_input(int **array_in, int *n_in)
{
    int n, *array;

    scanf("%d",&n);
    array=malloc(sizeof(int)*n);
    if(array == NULL)
    {
        perror("Malloc failed.\n");
        exit(100);
    }
    for(int i=0; i<n; i++){
        scanf("%d",&array[i]);
    }

    *array_in = array;
    *n_in = n;
    
    return ;
}/* read_input(int **array_in, int *n) */


int main(int argc, char *argv[])
{
    int id, p, n; /* id = rank of processor, p = no. of processes, n = no. of numbers */
    int *array = NULL ;
    MPI_Status status;
    int numbers[2];



    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if(id == 0){
        read_input(&array, &n);
    }

    /* Scatter Data to processors */
    /* Root node will send data whereas 
     * Others will receive */
    MPI_Scatter(array, 2 /*Send 2 bytes to everyone */, MPI_INT, 
                numbers, 2 /* Receive 2 bytes from root */, MPI_INT, 0 /* id of root node */,
                MPI_COMM_WORLD);


    int no1, no2;
    no1 = numbers[0];
    no2 = numbers[1];

    printf("ID : %d, No. of Processes: %d, no1 = %d, no2 = %d\n",id, p, no1, no2);


    if(id==0)
        free(array);

    MPI_Finalize();
    return 0;
}


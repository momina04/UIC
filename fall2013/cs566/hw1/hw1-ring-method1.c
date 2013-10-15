#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int id, p; /* id = rank of processor, p = no. of processes */

typedef struct work_t{
    int no1, no2;
}work_t;

typedef struct work_result_t{
    int min_no;
}work_result_t;


void do_work(work_t work, work_result_t *work_result)
{
    int no1, no2;

    no1 = work.no1;
    no2 = work.no2;

    work_result->min_no = no1 < no2? no1: no2;
    return ;
}/* do_work */

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



void master(MPI_Comm ring_comm)
{
    int n;
    int *array = NULL;
    int *subarray = NULL;
    int numbers[2];
    int lowest_no = 0;
    int lowest_no_so_far = 0;

    read_input(&array, &n);

    /* Let the slaves know too how many numbers we have to work on. */
    MPI_Bcast(&n /* Bcast n to everyone */, 1, MPI_INT, 0, ring_comm);

    lowest_no_so_far = array[0]; /* Assume the first number is the lowest */
    subarray = array;

    for(int i=0; i<floorf((float)n/(2*p)); i++){

        /* Scatter Data to processors including self */
        MPI_Scatter(subarray, 2 /*Send 2 bytes to everyone from array */, MPI_INT, 
                    numbers, 2 /* Receive 2 bytes from self */, MPI_INT, 0 /* id of root node */,
                    ring_comm);



        work_t work;
        work_result_t work_result;

        work.no1 = numbers[0];
        work.no2 = numbers[1];

        int min_no;

        do_work(work, &work_result);

        min_no = work_result.min_no;

        MPI_Reduce(&min_no, &lowest_no /* root receives 1 number from everyone and applies reduction operation on the way*/ ,1 /*1 number */, MPI_INT, 
                MPI_MIN , 0 /* id of root node */,
                ring_comm);

        if(lowest_no < lowest_no_so_far) lowest_no_so_far = lowest_no;

        //printf("Min [ ");
        for(int j=0; j<p*2; j+=2){
            //printf("(%d, %d) ,",subarray[j], subarray[j+1]);
        }
        //printf("\b ] = %d.\n", lowest_no);
        //printf("Lowest no so far is %d\n", lowest_no_so_far);

        subarray = subarray + p*2;
    }/*for*/
    printf("Lowest no is %d.\n", lowest_no_so_far);

    free(array);
    return ;
}/* master */


void slave(MPI_Comm ring_comm)
{
    int numbers[2];
    int n;

    /* Receive n from root node */
    MPI_Bcast(&n /* Receive n from root */, 1 /* rx 1 number */ , MPI_INT, 0, ring_comm);

    for(int i=0; i<floorf((float)n/(2*p)); i++){
        /* Receive 2 numbers from root */
        MPI_Scatter(NULL, 2 /*Send 2 bytes to everyone from array */, MPI_INT, 
                    numbers, 2 /* Receive 2 bytes from root */, MPI_INT, 0 /* id of root node */,
                    ring_comm);

        int min_no;
        work_t work;
        work.no1 = numbers[0];
        work.no2 = numbers[1];
        work_result_t work_result;
        do_work(work, &work_result);
        min_no = work_result.min_no;

        /* Slaves send the minimum of two numbers */
        MPI_Reduce(&min_no /* everyone sends 1 number to root */, NULL ,1 /* 1 number */, MPI_INT, 
                MPI_MIN , 0 /* id of root node */, 
                ring_comm);
    }/*for*/

    return ;
}/* slave */

int main(int argc, char *argv[])
{
    MPI_Status status;

    double start_time, end_time;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);


    /* Form a ring cartesian topology */
    int dims[]={p};
    int periods[]={1 /* true */ };
    int reorder = 1 /* false */;
    MPI_Comm ring_comm;

    if(MPI_Cart_create(MPI_COMM_WORLD, 1,  dims, periods, reorder, &ring_comm) < 0){
        perror("MPI_Cart_create failed. \n");
        MPI_Finalize();
        return -2;
    }

    MPI_Comm_rank(ring_comm, &id);

    MPI_Barrier(ring_comm);
    start_time = MPI_Wtime();

    if(id == 0){
        master(ring_comm);
    }
    else{
        slave(ring_comm);
    }


    MPI_Barrier(ring_comm);
    end_time = MPI_Wtime();


    if(id == 0)
        printf("Total time taken by parallel algorithm = %.6f\n", (end_time - start_time));

    MPI_Finalize();
    return 0;
}


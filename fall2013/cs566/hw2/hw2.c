#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int id, p; /* id = rank of processor, p = no. of processes */

typedef struct work_t{
    int *numbers;
    int n;
}work_t;

typedef struct work_result_t{
    int det;
}work_result_t;



int det(int *mat, int order)
{
    int debug = 0;
    double cofact[order], determinant, **temp;
    determinant = 0;
    debug=0;

    if(order==1)
    {
        determinant=mat[0*order+0];

        if(debug==1)
        {
            printf("order 1 if\n");
        }
    }
    else if(order==2)
    {
        determinant= ((mat[0*order+0]*mat[1*order+1])-(mat[0 * order + 1]*mat[1 * order + 0]));

        if(debug==1)
        {
            printf("order 2 if\n");
        }
    }
    else
    {

        int column, rowtemp, coltemp, colread;
        for (column=0; column<order; column++) 
        {
            /* Now create an array of size N-1 to store temporary data used for calculating minors */
            temp= malloc((order-1)*sizeof(*temp)); 
            for(rowtemp=0; rowtemp<(order-1); rowtemp++)
            {
                /* Now asign each element in the array temp as an array of size N-1 itself */
                temp[rowtemp]=malloc((order-1)*sizeof(double));
            }
            for(rowtemp=1; rowtemp<order; rowtemp++)
            {
                /* We now have our empty array, and will now fill it by assinging row and collumn values    from the original mat with the aprroriate elements excluded */
                coltemp=0;
                for(colread=0; colread<order; colread++)
                {
                    /* When the collumn of temp is equal to the collumn of the matrix, this indicates this row should be exlcuded and is skiped over */
                    if(colread==column)
                    {
                        continue;
                    }
                    temp[rowtemp-1][coltemp] = mat[rowtemp*order+colread];
                    coltemp++;
                }

            }
            if(debug==1)
            {
                printf("column =%d, order=%d\n", column, order);
            }
            determinant+=(mat[0 * order + column]*(1 - 2*(column & 1))*det(temp, order-1));
        }   

    }  
    return(determinant);   
}

void do_work(work_t work, work_result_t *work_result)
{
    int d;
    d = det(work.numbers, work.n);
    work_result->det = d;
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



void master(MPI_Comm mesh_comm)
{
    int n;
    int *array = NULL;
    int *numbers;
    int lowest_no = 0;
    int lowest_no_so_far = 0;

    read_input(&array, &n);


    /* Let the slaves know too how many numbers we have to work on. */
    MPI_Bcast(&n /* Bcast n to everyone */, 1, MPI_INT, 0, mesh_comm);

    numbers = malloc(sizeof(int) * (n/p));
    /* Scatter Data to processors including self */
    MPI_Scatter(array, n/p  /*Send n/p numbers to everyone from array */, MPI_INT, 
                numbers, n/p /* Receive n/p numbers from self */, MPI_INT, 0 /* id of root node */,
                mesh_comm);

    work_t work;
    work_result_t work_result;

    work.n = n/p;
    work.numbers = numbers;


    do_work(work, &work_result);


    interm_matrix = malloc(sizeof(int) * p);
    MPI_Gather(&work_result->det, 1 , MPI_INT , interm_matrix, 1, MPI_INT, 0, mesh_comm)

    work.n = sqrt(p);
    work.numbers = interm_matrix;

    do_work(work, &work_result);

    printf("Determinant of Matrix %d.\n", work_result.det);

    free(numbers);
    free(interm_matrix);

    free(array);
    return ;
}/* master */


void slave(MPI_Comm mesh_comm)
{
    int n;
    int *numbers;
    int lowest_no = 0;
    int lowest_no_so_far = 0;



    /* Let the slaves know too how many numbers we have to work on. */
    MPI_Bcast(&n /* Bcast n to everyone */, 1, MPI_INT, 0, mesh_comm);

    numbers = malloc(sizeof(int) * (n/p));
    /* Scatter Data to processors including self */
    MPI_Scatter(NULL, n/p  /*Send n/p numbers to everyone from array */, MPI_INT, 
                numbers, n/p /* Receive n/p numbers from self */, MPI_INT, 0 /* id of root node */,
                mesh_comm);

    work_t work;
    work_result_t work_result;

    work.n = n/p;
    work.numbers = numbers;


    do_work(work, &work_result);


    MPI_Gather(&work_result->det, 1 , MPI_INT , NULL, 1, MPI_INT, 0, mesh_comm)

    free(numbers);

    return ;
}/* slave */

int main(int argc, char *argv[])
{
    MPI_Status status;

    double start_time, end_time;
    int ndims =2 ;
    int PROCS_PER_DIM = 0;
    PROCS_PER_DIM = sqrt(p);


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);





    /* Form a MESH cartesian topology */
    printf("Procs in each dimenstion = %d\n",PROCS_PER_DIM);

    /* Allocate and initialize dims and periods array */
    int *dims = NULL;
    dims = malloc(sizeof(int) * ndims);
    int *periods = NULL;
    periods = malloc(sizeof(int) * ndims);
    for(int k=0; k<ndims;k++){
        dims[k]= PROCS_PER_DIM;
        periods[k]= 0 /* false */;
    }

    int reorder = 1 /* false */;
    MPI_Comm mesh_comm;

    //printf("log0\n");
    if(MPI_Cart_create(MPI_COMM_WORLD, ndims,  dims, periods, reorder, &mesh_comm) < 0){
        perror("MPI_Cart_create failed. \n");
        MPI_Finalize();
        return -2;
    }

    //printf("log1\n");

    free(dims);
    free(periods);
    //printf("log2\n");

    MPI_Comm_rank(mesh_comm, &id);

    if(id == 0)
    printf("Processors(p) = %d , Dimensions(d) = %d\n", p, ndims);

    MPI_Barrier(mesh_comm);
    start_time = MPI_Wtime();

    //printf("log3\n");
    if(id == 0){
        master(mesh_comm);
    }
    else{
        slave(mesh_comm);
    }

    //printf("log4\n");

    MPI_Barrier(mesh_comm);
    end_time = MPI_Wtime();


    if(id == 0)
        printf("Total time taken by parallel algorithm = %.6f\n", (end_time - start_time));

    MPI_Finalize();
    return 0;
}

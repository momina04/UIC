#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int id, p, n;
    MPI_Status status;

    int array[38];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if(id==0){
        /* Initialize */
        for(int i=0; i<8; i++)
        {
            array[i]=i+1;
        }
        /* Send data to each processor */
        {
            MPI_Send(&array[(id+1)*2], 8-((id+1)*2), MPI_INT, id+1, 007, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&array[id*2], 8-(id*2), MPI_INT, id-1, 007, MPI_COMM_WORLD, &status);
        if(id<3)
            MPI_Send(&array[(id+1)*2], 8-((id+1)*2), MPI_INT, id+1, 007, MPI_COMM_WORLD);
    }

    int no1, no2;
    no1 = array[id*2];
    no2 = array[(id*2)+1];

    printf("ID : %d, Size: %d, no1 = %d, no2 = %d\n",id, p, no1, no2);



    MPI_Finalize();
    return 0;
}


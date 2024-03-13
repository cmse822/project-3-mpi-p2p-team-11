#include <mpi.h>
#include <stdio.h> 
#include <cstdlib>
#include <cmath>

double nonblocking_ping_pong(int rank, int nTrials, char* buffer, int buffSize ) {
    double start, end;

    char recvBuff(buffSize);

    for (int i = 0; i < nTrials; ++i) {
        // skip timing for first few iterations
        start = MPI_Wtime();
        if ( rank == 0 ){
            MPI_Request reqst[2];
            // communicating rank 0 to rank 1
            MPI_Isend(buffer, buffSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &reqst[0]); 
            MPI_Irecv(&recvBuff, buffSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE); 
        }
        else {
            MPI_Request reqst[2];
            // communicating rank 1 to 0
            MPI_Irecv(&recvBuff, buffSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqst[0]);
            MPI_Isend(buffer, buffSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE); 
        }
    }

    end = MPI_Wtime();

    return (end - start);
}


int main(int argc, char *argv[]) {
           
    MPI_Init(&argc, &argv); 
    MPI_Comm comm = MPI_COMM_WORLD;
      
    int rank, buffSize, nTrials = 10000;
    char buffer;    
    double time;
    MPI_Comm_rank(comm, &rank);
    for (int j = 0; j < 8; ++j) {
        buffSize = pow(2, j);
        time = nonblocking_ping_pong(rank, nTrials, &buffer, buffSize);
        printf("The total communication time is %6.9f\n",time);
    }
    
    MPI_Finalize(); 
    
    return 0; 
}

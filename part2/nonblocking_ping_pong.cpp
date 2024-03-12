#include <mpi.h>

double nonblocking_ping_pong(int rank, int nTrials, char* buffer, int buffSize ) {
    double start, end;

    char recvBuff(buffSize);

    for (int i = 0; i < nTrials; ++i) {
        // skip timing for first few iterations
        if ( i == 15 ) start = MPI_Wtime();
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

    return end - start;
}
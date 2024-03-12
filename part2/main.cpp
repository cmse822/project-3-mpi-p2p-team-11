#include <mpi.h>
#include <stdio.h> // Include for printf
#include <cstdlib>

int main(int argc, char *argv[]) {
           
    MPI_Init(&argc, &argv); // Initialize the MPI environment
    MPI_Comm comm = MPI_COMM_WORLD;
      
    int rank, nTrials = 200, buffSize = 16;
    char buffer;    
    double time;
    MPI_Comm_rank(comm, &rank);
    time = nonblocking_ping_pong(rank, nTrials, &buffer, buffSize);
    printf("The total communication time is %6.9f\n",time);    
    
    MPI_Finalize(); // Finalize the MPI environment
    
    return 0; // Successful execution
}


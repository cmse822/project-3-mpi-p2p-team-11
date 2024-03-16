#include <mpi.h>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <fstream>

double nonblocking_ping_pong(int rank, int nTrials, const std::vector<char>& sendBuffer, int buffSize) {
    double start = 0.0, end = 0.0;

    std::vector<char> recvBuff(buffSize);

    // Warm-up phase to establish communication path, not timed
    for (int i = 0; i < std::min(10, nTrials); ++i) {
        if (rank == 0) {
            //non-blocking send and recv (rank 0)
            MPI_Request reqst[2]; //local within if scope
            MPI_Isend(sendBuffer.data(), buffSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &reqst[0]);
            MPI_Irecv(recvBuff.data(), buffSize, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE);
        } else {
            //non-blocking send and recv (rank 1)
            MPI_Request reqst[2]; //local within else cope
            MPI_Irecv(recvBuff.data(), buffSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqst[0]);
            MPI_Isend(sendBuffer.data(), buffSize, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE); //wait for send and recv
        }
    }

    // Timing starts after warm-up
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    for (int i = 0; i < nTrials; ++i) {
        if (rank == 0) {
            //non-blocking send and recv (rank 0)
            MPI_Request reqst[2];
            MPI_Isend(sendBuffer.data(), buffSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &reqst[0]);
            MPI_Irecv(recvBuff.data(), buffSize, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE); //wait for send and recv
        } else {
            //non-blocking send and recv (rank 1)
            MPI_Request reqst[2];
            MPI_Irecv(recvBuff.data(), buffSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqst[0]);
            MPI_Isend(sendBuffer.data(), buffSize, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &reqst[1]);
            MPI_Waitall(2, reqst, MPI_STATUS_IGNORE); //wait for send and recv
        }
    }
    end = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);

    return (end - start) / nTrials; // Return average time per trial
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, buffSize, nTrials = 100;
    double time;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::ofstream outputFile;
    
    if (rank == 0) {
        outputFile.open("nonblocking_p2p_timing_results.csv");
        outputFile << "BufferSize,Time\n";
    }
    //vary message sizes 2^1 to 2^21
    for (int j = 1; j <= 21; ++j) {
        buffSize = static_cast<int>(pow(2, j));
        std::vector<char> buffer(buffSize, 'a'); // Dynamically sized send buffer, filled with 97 ('a')
        time = nonblocking_ping_pong(rank, nTrials, buffer, buffSize);
        
        if (rank == 0) {
            outputFile << buffSize << "," << time << "\n";
        }
    }
    
    if (rank == 0) {
        outputFile.close();
    }

    MPI_Finalize();
    return 0;
}

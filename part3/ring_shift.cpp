#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(argc, argv);

    int* sendBuffer;
    int* recvBuffer;
    int sendCount;
    int recvCount;
    MPI_Datatype sendType;
    MPI_Datatype recvType;
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    int numRanks;
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
    int shiftCount;
    if (myRank == 0)
    {
        shiftCount = rand() % numRanks;
        
    }
    MPI_Bcast(&shiftCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int dest = (myRank + shiftCount) % numRanks;
    int source = (myRank - shiftCount) % numRanks;
    MPI_Status status;

    MPI_Sendrecv(sendBuffer, sendcount, sendtype, dest, 0, recvBuffer, recvCount, recvType, source, 0, MPI_COMM_WORLD, status);
}

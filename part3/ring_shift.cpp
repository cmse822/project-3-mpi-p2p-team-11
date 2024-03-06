#include <time.h>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

void init_data(std::vector<uint8_t>& buff)
{
    std::fill(buff.begin(), buff.end(), 0);
    for (size_t i = 0; i < buff.size(); i ++)
    {
        buff[i] = (rand() % 26) + 65;
    }
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    int numRanks;
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
    srand(time(NULL) + myRank);
    
    std::cout << "NUMRANKS = " << numRanks << "; MYRANK = " << myRank << std::endl;
    int shiftCount;
    if (myRank == 0)
    {
        shiftCount = rand() % numRanks;
        std::cout << "Shift Count: " << shiftCount << std::endl;
    }
    MPI_Bcast(&shiftCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    std::vector<uint8_t> sendBuffer;
    std::vector<uint8_t> recvBuffer;

    MPI_Datatype sendType = MPI_CHAR;
    MPI_Datatype recvType = MPI_CHAR;
    int dest = (myRank + shiftCount) % numRanks;
    int source = (myRank - shiftCount) % numRanks;
    MPI_Status status;

    std::ostringstream out;
    for (size_t i = 2; i <= 16; i *= 2) 
    {
        sendBuffer.resize(i);
        recvBuffer.resize(i);
        init_data(sendBuffer);
        out << "Rank " << myRank << " before: ";
        for (auto ch : sendBuffer)
        {
            out << ch;
        }
        out << std::endl;
        std::cout << out.str();
        out.str("");
        MPI_Sendrecv(sendBuffer.data(), i, sendType, dest, 0, recvBuffer.data(), i, recvType, source, 0, MPI_COMM_WORLD, &status);
        out << "Rank " << myRank << " after: ";
        for (auto ch : recvBuffer)
        {
            out << ch;
        }
        out << std::endl;
        std::cout << out.str();
        out.str("");
    }

    MPI_Finalize();
}

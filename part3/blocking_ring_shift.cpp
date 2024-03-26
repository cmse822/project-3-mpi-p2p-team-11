#include <time.h>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>

void init_data(std::vector<uint8_t>& buff)
{
    std::fill(buff.begin(), buff.end(), 0);
    for (size_t i = 0; i < buff.size(); i ++)
    {
        // Populate with random characters
        buff[i] = (rand() % 26) + 65;
    }
}

void init_trials(std::vector<std::vector<uint8_t>>& sendBuffers, std::vector<std::vector<uint8_t>>& recvBuffers, size_t numTrials, size_t buffSize)
{
    sendBuffers.clear();
    recvBuffers.clear();
    // Initialize seperate data for all trials
    for (size_t i = 0; i < numTrials; i++)
    {
        std::vector<uint8_t> sendBuffer;
        std::vector<uint8_t> recvBuffer;
        sendBuffer.resize(buffSize);
        recvBuffer.resize(buffSize);
        init_data(sendBuffer);
        sendBuffers.push_back(sendBuffer);
        recvBuffers.push_back(recvBuffer);
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
    
    // Randomly determine the distance of the shift
    // The shift still only uses a single sendrecv
    int shiftCount;
    if (myRank == 0)
    {
        shiftCount = rand() % (numRanks - 1) + 1;
    }
    MPI_Bcast(&shiftCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    std::vector<std::vector<uint8_t>> sendBuffers;
    std::vector<std::vector<uint8_t>> recvBuffers;
    
    MPI_Datatype sendType = MPI_CHAR;
    MPI_Datatype recvType = MPI_CHAR;
    int dest = (myRank + shiftCount) % numRanks;
    int source = (myRank - shiftCount + numRanks) % numRanks;
    
    MPI_Status status;
    
    std::vector<double> wTimes;

    double startTime, endTime;
    const size_t numTrials = 100;
    for (size_t i = 0; i < 20; i++) // Run trials with messages of size 2^(i+1)
    {
        init_trials(sendBuffers, recvBuffers, numTrials, round(pow(2, i + 1)));
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all process start at the same time for the shift
        startTime = MPI_Wtime();
        for (size_t j = 0; j < numTrials; j++) // Run 100 trials at each size
        {
            MPI_Sendrecv(sendBuffers[j].data(), round(pow(2, i+1)), sendType, dest, 0, recvBuffers[j].data(), round(pow(2, i+1)), recvType, source, 0, MPI_COMM_WORLD, &status);
            MPI_Barrier(MPI_COMM_WORLD); // Used to ensure all processes have finished their shift
        }
        endTime = MPI_Wtime();
        wTimes.push_back(endTime - startTime);
    }
    
    MPI_Finalize();

    if (myRank == 0)
    {
        std::ofstream fstream("Blocking_Ring_Shift_Wall_Times.txt", std::ios_base::app); // Append to file
        for (size_t i = 0; i < wTimes.size(); i++)
        {
            // Format: Num Ranks    Message Size 2^N    Time (s)
            fstream << numRanks << "\t" << i + 1 << "\t" << wTimes[i] << std::endl;
        }
        fstream.close();
    }
}

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv)

{


MPI_Init(&argc, &argv);
MPI_Comm comm = MPI_COMM_WORLD;


int rank_id;
MPI_Comm_rank(comm, &rank_id);

double ov_time, start_, end_;

for(int i=2;i<=2097152;i=i*2)
{
ov_time = 0;

//100 iterations per message size
for(int ii=0;ii<100;ii++)
{

char *buf_s = (void *)malloc(sizeof(char)*i);
char *buf_r = (void *)malloc(sizeof(char)*i);

int j=0;
for(;j<i;j++){ buf_s[j] = 'a'; buf_r[j] = 0; }


MPI_Barrier(comm);
start_ = MPI_Wtime();
if(rank_id==0)
{
//MPI process 0
MPI_Send(buf_s,i,MPI_CHAR,1,0,comm);
MPI_Recv(buf_r,i,MPI_CHAR,1,0,comm,MPI_STATUS_IGNORE);
}
else
{
//MPI process 1
MPI_Recv(buf_r,i,MPI_CHAR,0,0,comm,MPI_STATUS_IGNORE);
MPI_Send(buf_s,i,MPI_CHAR,0,0,comm);
}

end_ = MPI_Wtime();
ov_time += (end_-start_);

free(buf_s);
free(buf_r);

}
ov_time = ov_time / 100.0;
printf("%f \n",ov_time);

}

MPI_Finalize();


return 0;

}

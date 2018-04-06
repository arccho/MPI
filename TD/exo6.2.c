#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int wrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int N = atoi(argv[1]);
	int i;
	
	
	for(i=0; i < N; i++)
	{
		if(wrank == i%world_size)
		{
			FILE* f = fopen("out.txt", "a+");
			fprintf(f, "%d (rang %d)\n", i, wrank);
			fclose(f);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	

	MPI_Finalize();
	return 0;
}

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	if (argc!=2)
	{
		printf("Donner un nombre de simulation en paramètre.\n");
		return 0;
	}

	int simCount = atoi(argv[1]);
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	srand(time(NULL) + wrank);
	long total = 0;
	for (int sim=0; sim<simCount; ++sim)
	{
		long count = 1;
		while (rand()%52!=0)
			++count;

		total += count;
	}

	long sum;
	MPI_Reduce(&total, &sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

	if (wrank==0)
	{
		double result = (double) sum / (world_size*simCount);
		printf("Result : %f\n", result);
	}

	MPI_Finalize();
	return 0;
}



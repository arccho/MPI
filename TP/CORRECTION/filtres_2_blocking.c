#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TSIZE 1000000

void filter(double* tab)
{
	for (int i=1; i<TSIZE-1; ++i)
		tab[i] = (tab[i-1]+tab[i]+tab[i+1]) / 3;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	if (world_size!=2)
	{
		printf("Ne marche qu'avec exactement 2 processus.\n");
		MPI_Finalize();
		return 0;
	}

	srand(time(NULL) + wrank);
	double* common = calloc(TSIZE, sizeof(double));
	double* specific;
	if (wrank==1)
		specific = calloc(TSIZE, sizeof(double));

	double totalTime = 0;
	for (int tag=0; tag<100; ++tag)
	{
		if (wrank==0)
		{
			for (int i=0; i<TSIZE; ++i)
				common[i] = (double) rand() / RAND_MAX;

			MPI_Ssend(common, TSIZE, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
		}
		else
		{
			for (int i=0; i<TSIZE; ++i)
				specific[i] = (double) rand() / RAND_MAX;

			double start = MPI_Wtime();
			filter(specific);
			MPI_Recv(common, TSIZE, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			double mean = 0;
			for (int i=0; i<TSIZE; ++i)
				mean += common[i];

			mean /= TSIZE;
			totalTime += MPI_Wtime()-start;
		}
	}

	if (wrank==1)
	{
		printf("%f\n", totalTime);
		free(specific);
	}

	free(common);

	MPI_Finalize();
	return 0;
}


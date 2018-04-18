#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	struct
	{
		int dice;
		int rank;
	} in, out;

	in.rank = wrank;

	srand(time(NULL) + wrank);
	int total = 0, sum;
	while (1)
	{
		in.dice = rand()%6 + 1;

		MPI_Reduce(&in.dice, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		total += sum;

		MPI_Allreduce(&in, &out, 1, MPI_2INT, MPI_MAXLOC, MPI_COMM_WORLD);
		if (out.dice==6)
			break;
	}

	if (wrank==0)
		printf("Total : %d\n", total);

	MPI_Finalize();
	return 0;
}


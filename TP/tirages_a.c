#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int wrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	srand(time(NULL) + wrank);

	int dice;
	int maxdice;
	int notfinished = 1;
	int total = 0;
	int sum;



	while (notfinished)
	{
        dice = rand()%6 + 1;

        //processus 0 do the sum of all dices
        MPI_Reduce(&dice, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		total += sum;

        //All processus get the max value
		MPI_Allreduce(&dice, &maxdice, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
		if (maxdice==6)
		{
            notfinished = 0;
		}
	}

    if (wrank==0)
    {
    		printf("Total : %d\n", total);
    }

	MPI_Finalize();
	return 0;
}

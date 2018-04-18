#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	if (argc!=2)
	{
		printf("Donner un nombre de simulation en paramètre.\n");
	}

	long NbSimu = atoi(argv[1]);

	MPI_Init(&argc, &argv);

	int wrank;
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	srand(time(NULL) + wrank);

	long total = 0;
	long sim;

    for(sim=0; sim<NbSimu; ++sim)
	{
		long Nb = 1;
		while (rand()%52!=0)    //0 représente la bonne carte
		{
            ++Nb;
		}
		total += Nb;
	}

    long totalEssai;
	MPI_Reduce(&total, &totalEssai, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);


    if (wrank==0)
    {
		double moyenne = (double) totalEssai / (world_size*NbSimu);
		printf("Moyenne theorique =52\n Moyenne des moyenne pour avoir un 2 de trèfle : %f\n", moyenne);
    }

	MPI_Finalize();
	return 0;
}

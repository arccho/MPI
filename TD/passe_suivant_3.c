#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Nombre de cycles en argument.\n");
		return 0;
	}

	int nbPasses = atoi(argv[1]);
	if (nbPasses<1)
		return 0;

	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	int passes = 0;
	int source = (wrank-1+world_size) % world_size;
	int dest = (wrank+1) % world_size;

	int cycleCount = nbPasses / world_size;
	int reste = nbPasses % world_size;

//On traite les cycles complets
	if (wrank==0)
	{
			int cycle;
			for (cycle=0; cycle<cycleCount; ++cycle)
		{
			MPI_Ssend(&passes, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
			MPI_Recv(&passes, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			++passes;
			printf("Je suis %d. J'ai reçu de %d. Nombre de passes : %d\n", wrank, source, passes);
		}
	}
	else
	{
		int cycle1;
		for (cycle1=0; cycle1<cycleCount; ++cycle1)
		{
			MPI_Recv(&passes, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			++passes;
			printf("Je suis %d. J'ai reçu de %d. Nombre de passes : %d\n", wrank, source, passes);
			MPI_Ssend(&passes, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}
		
	}

//On traite ce qui reste 
	if(reste > 0) {
	if(wrank == 0){
		MPI_Ssend(&passes, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}else if(wrank < reste){
		MPI_Recv(&passes, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		++passes;
		printf("Je suis %d. J'ai reçu de %d. Nombre de passes : %d\n", wrank, source, passes);
		MPI_Ssend(&passes, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}else if( wrank == reste){
		MPI_Recv(&passes, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		++passes;
		printf("Je suis %d. J'ai reçu de %d. Nombre de passes : %d\n", wrank, source, passes);
	}
}

	MPI_Finalize();
	return 0;
}

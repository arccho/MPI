#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TSIZE 1000000

void filtre(double *tab)
{
    int boucle;
	for (boucle=1; boucle<TSIZE-1; boucle++)
	{
        tab[boucle] = (tab[boucle-1]+tab[boucle]+tab[boucle+1]) / 3;
	}
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
	double* tab_commun = calloc(TSIZE, sizeof(double));
	double* tab_specific;
	if (wrank==0)
	{
        tab_specific = calloc(TSIZE, sizeof(double));
	}

	double totalTime = 0;
	int etape;
	int boucle;
	for (etape=0; etape<100; etape++)
	{
		if (wrank==0)//processus 0
		{
			for (boucle=0; boucle<TSIZE; boucle++) //Initialise tab à envoyer de double random
			{
                tab_commun[boucle] = (double) rand() / RAND_MAX;
			}

			double start = MPI_Wtime();
			//Envoie à P1
			MPI_Ssend(tab_commun, TSIZE, MPI_DOUBLE, 1, etape, MPI_COMM_WORLD);

			for (boucle=0; boucle<TSIZE; boucle++)
            {
                tab_specific[boucle] = (double) rand() / RAND_MAX;
            }


			filtre(tab_specific);
			totalTime += MPI_Wtime()-start;
		}
		else    //processus 1
		{
            //Recoit le tab envoyé par P0
			MPI_Recv(tab_commun, TSIZE, MPI_DOUBLE, 0, etape, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			filtre(tab_commun);
		}
	}

	if (wrank==0)
	{
		printf("%f\n", totalTime);
		free(tab_specific);
	}

	free(tab_commun);

	MPI_Finalize();
	return 0;
}

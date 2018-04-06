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

	int i, j;

	int nbrand = 0;
	short *tabrandom;
	int tab[10];

	if(wrank%2 == 0)//rang pair
	{	
		nbrand = rand()%300 +100;
		tabrandom = (short*) malloc(nbrand * sizeof(short));
		for(i=0; i < nbrand; i++)
		{
			tabrandom[i] = rand()%10;
		}	
		
		if(wrank < world_size-1)//pas dernier processus
		{
			MPI_Ssend(&nbrand, 1, MPI_INT, wrank+1, 0, MPI_COMM_WORLD);
			MPI_Ssend(tabrandom, nbrand, MPI_SHORT, wrank+1, 1, MPI_COMM_WORLD);
		}
		else
		{	printf("je suis %d et voici mon tableau: ", wrank);
			for(i=0; i < 10; i++)
			{
				for(j=0; j < nbrand; j++)
				{
					if(tabrandom[j] == i)
					{
						tab[i] += 1;
					}
				}
				printf("%d ", tab[i]);
			}
printf("\n");

		}
	}
	else //rang impair
	{
		MPI_Recv(&nbrand, 1, MPI_INT, wrank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("je suis %d et j'ai recu de %d\n", wrank, wrank-1);
		tabrandom = (short*) malloc(nbrand * sizeof(short));
		MPI_Recv(tabrandom, nbrand, MPI_SHORT, wrank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("je suis %d, j'ai recu de %d et voici mon tableau: ", wrank, wrank-1);
		for(i=0; i < 10; i++)
		{
			for(j=0; j < nbrand; j++)
			{
				if(tabrandom[j] == i)
				{
					tab[i] += 1;
				}
			}
			printf("%d ", tab[i]);
		}
		printf("\n");
	}

	MPI_Finalize();

	return 0;
}

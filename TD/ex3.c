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

	int arrayInteger[10];
	char arrayChar[10];
	int sum=0;


	int parcour;
	int i, j;

	if(wrank==0)
	{	
		for(parcour = 1; parcour < world_size; parcour++)
		{
			if (parcour%2 == 0)//pair
			{
				printf("ajout des rand integers\t");
				for(i = 0; i < 10; ++i)
				{	
					arrayInteger[i] = rand()%10;
					printf("%d ",arrayInteger[i]);
				}
				printf("pour %d\n", parcour);
				MPI_Ssend(&arrayInteger, 10, MPI_INT, parcour, 1, MPI_COMM_WORLD);
				MPI_Recv(&sum, 1, MPI_INT, parcour, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("Retour somme de %d = %d\n", parcour, sum);
			}
			else//impair
			{
				printf("ajout des rand chars\t");
				for(j = 0; j < 10; ++j)
				{	
					arrayChar[j] = 'a' + (rand()%26);
					printf("%c ",arrayChar[j]);
				}
				printf("pour %d\n", parcour);	
				MPI_Ssend(&arrayChar, 10, MPI_CHAR, parcour, 2, MPI_COMM_WORLD);
				MPI_Recv(&arrayChar, 10, MPI_CHAR, parcour, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("Retour tab char de %d\t", parcour);
				for(j = 0; j < 10; ++j)
				{	
					printf("%c ",arrayChar[j]);
				}
				printf("\n");
			}
		}		
	}

	if(wrank%2 == 0) //pair
	{
		if(wrank != 0)
		{
			MPI_Recv(&arrayInteger, 10, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Je suis %d (pair) et j'ai recu  de %d\t", wrank,  0);
			for(i = 0; i < 10; ++i)
			{	
				//printf("%d ",arrayInteger[i]);
				sum += arrayInteger[i];
			}
			//printf("\n");
			MPI_Ssend(&sum, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);

		}
		
	}else if (wrank != 0) //impair
	{
		MPI_Recv(&arrayChar, 10, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("Je suis %d (impair) et j'ai recu  de %d\t", wrank,  0);
		for(j = 0; j < 10; ++j)
		{	
			//printf("%c ", arrayChar[j]);
			arrayChar[j] = arrayChar[j] + (int)'A' - (int)'a';
		}
		//printf("\n");
		MPI_Ssend(&arrayChar, 10, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}

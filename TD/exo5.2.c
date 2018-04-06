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
	int count = 0;
	int ok = 0;
	int nbrand;
	srand( time ( NULL )+ wrank );
	int tab[world_size];
	
	while(ok == 0)
	{
		count++;
		nbrand = rand()%2;
		MPI_Allgather(&nbrand, 1, MPI_INT, tab, 1, MPI_INT, MPI_COMM_WORLD);


		ok = 1;
		for(i=0; i < world_size; i++)
		{
			if(nbrand != tab[i])
			{
				ok = 0;
			}
		}
		
		if(count > atoi(argv[1])){ok = 1;}
	}
	if(wrank == 0)//processus 0
	{
		printf("%d tentatives faites\n", count);
	}
	MPI_Finalize();
	return 0;
}

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
	int nbrand;
	srand( time ( NULL )+ wrank );
	int tab[world_size];
	
	for(i=0; i < world_size; i++)
	{
		if(wrank == i)
		{
			FILE* f = fopen("out.txt", "a+");
			fprintf(f, "je suis le rang %d.\n", wrank);
			fclose(f);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	

	MPI_Finalize();
	return 0;
}

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void jouage(MPI_Comm COMM_G, int* color)
{
	int rank;
	MPI_Comm_rank(COMM_G, &rank);

	struct
	{
		int dice;
		int rank;
	} in, out;

	in.rank = rank;
	short wins[4] = {0, 0, 0, 0};

	while(1)
	{
		in.dice = rand()%6;
		// Si deux processus ont la même valeur de 'dice', le processus de rang le plus petit gagne.
		MPI_Allreduce(&in, &out, 1, MPI_2INT, MPI_MAXLOC, COMM_G);
		++wins[out.rank];
		if (wins[out.rank]==3)
			break;
	}

	if (rank==out.rank)
		*color = 0;
	else
		*color = 1;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	if (world_size!=16)
	{
		printf("Nécessite 16 processus exactement.\n");
		MPI_Finalize();
		return 0;
	}

	int color = wrank / 4;

	// On garde ainsi l'ordre relatif des processus dans le nouveau communicateur.
	int key = wrank;

	// Il n'y a qu'une seule variable pour le nouveau communicateur, mais
	// sa définition diffère selon les processus.
	MPI_Comm COMM_G;
	MPI_Comm_split(MPI_COMM_WORLD, color, key, &COMM_G);

	srand(time(NULL) + wrank);
	jouage(COMM_G, &color);
	if (color==0)
		printf("Vainqueur du premier tour : %d\n", wrank);

	MPI_Comm_split(MPI_COMM_WORLD, color, key, &COMM_G);
	if (color==0)
		jouage(COMM_G, &color);

	if (color==0)
		printf("Vainqueur : %d\n", wrank);

	MPI_Finalize();
	return 0;
}


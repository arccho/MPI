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

	int i;
	srand( time ( NULL )+ wrank );
	int size = 10000;
	float total = 0;
	float tab[size];
	if(wrank == 0)
	{
		for(i=0; i < size; i++)
		{
			tab[i] = ((float)rand()/(float)(RAND_MAX)) * 11;
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);    //fin initialisation tableau


	//calcul distribué
	float mini_total = 0;
	if(size%world_size == 0)//scatter
	{
		float part_tab[size/world_size];
		MPI_Scatter(tab, size/world_size, MPI_FLOAT, part_tab, size/world_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
		for(i=0; i < size/world_size; i++)
		{
			mini_total += part_tab[i];
		}
		printf("somme de processus %d = %f\n", wrank, mini_total);
		MPI_Reduce(&mini_total, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		if(wrank==0){printf("Total distribué = %f\n", total);}
	}
	else//scatterv
	{
	    int size_cut = (int)(size/world_size);
	    int recvcount;
        int sendcounts[world_size];
        int displs[world_size];

        for(i=0; i < world_size; i++)
        {
            sendcounts[i] = size_cut;
            displs[i] = i * size_cut;
        }
        sendcounts[world_size-1] = size_cut + size%world_size;

        if (wrank== world_size-1)
        {
            size_cut = (int)(size/world_size) + size%world_size;
        }
        float recvbuf[size_cut];
        recvcount = size_cut;
        MPI_Scatterv(tab, sendcounts, displs, MPI_FLOAT, recvbuf, recvcount, MPI_FLOAT, 0, MPI_COMM_WORLD);
        for(i=0; i < size_cut; i++)
		{
			mini_total += recvbuf[i];
		}
		printf("somme de processus %d = %f\n", wrank, mini_total);
		MPI_Reduce(&mini_total, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		if(wrank==0){printf("Total distribué = %f\n", total);}
	}
	MPI_Barrier(MPI_COMM_WORLD);    //fin distribué

	total = 0;
	//calcul processus 0 uniquement
	if(wrank == 0)
	{
		for(i=0; i < size; i++)
		{
			total += tab[i];
		}
		printf("Total centralisé = %f\n", total);
	}

	MPI_Finalize();
	return 0;
}

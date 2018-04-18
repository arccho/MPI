#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALPHABET 26
#define FIELDS 4
#define TSIZE 1000

typedef struct Data_
{
	char id;
	double data[TSIZE];
	short pMean;
	short pMedian;
} Data;

MPI_Datatype createStruct()
{
	int blocklengths[FIELDS] = {1, TSIZE, 1, 1};
	MPI_Aint displs[FIELDS];
	displs[0] = offsetof(Data, id);
	displs[1] = offsetof(Data, data);
	displs[2] = offsetof(Data, pMean);
	displs[3] = offsetof(Data, pMedian);

	for (int i=0; i<FIELDS; ++i)
		printf("%ld ", displs[i]);
	printf("%lu\n", sizeof(Data));

	MPI_Datatype types[FIELDS] = {MPI_CHAR, MPI_DOUBLE, MPI_SHORT, MPI_SHORT};
	MPI_Datatype MPI_Data;

	MPI_Type_create_struct(FIELDS, blocklengths, displs, types, &MPI_Data);
	MPI_Type_commit(&MPI_Data);

	return MPI_Data;
}

void setProperties(Data* pData)
{
	double sum = 0;
	int pCount = 0;
	for (int i=0; i<TSIZE; ++i)
	{
		sum += pData->data[i];
		if (pData->data[i] > 0)
			++pCount;
	}

	if (sum > 0)
		pData->pMean = 1;
	else
		pData->pMean = 0;

	if (pCount > TSIZE/2)
		pData->pMedian = 1;
	else
		pData->pMedian = 0;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	MPI_Datatype MPI_Data = createStruct();

	Data* dataSet;
	int load = ALPHABET / world_size;
	int left = 0;
	if (wrank==0)
	{
		srand(time(NULL));
		dataSet = calloc(ALPHABET, sizeof(Data));
		for (int letter=0; letter<ALPHABET; ++letter)
		{
			dataSet[letter].id = 'A'+letter;
			for (int i=0; i<TSIZE; ++i)
				dataSet[letter].data[i] = ((double) rand() / RAND_MAX)*2 - 1;
		}

		left = ALPHABET - load*world_size;
	}
	else
		dataSet = calloc(load, sizeof(Data));

	MPI_Scatter(dataSet+left, load, MPI_Data, dataSet+left, load, MPI_Data, 0, MPI_COMM_WORLD);

	load += left;
	for (int i=0; i<load; ++i)
		setProperties(dataSet+i);

	int count = 0;
	for (int i=0; i<load; ++i)
	{
		Data* pData = dataSet+i;
		if (pData->pMean != pData->pMedian)
			++count;
	}

	int result;
	MPI_Reduce(&count, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (wrank==0)
		printf("%d\n", result);

	free(dataSet);
	MPI_Finalize();
	return 0;
}


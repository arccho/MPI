#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Matrix_
{
	int rows;
	int cols;
	int* data;
} Matrix;

Matrix createMatrix(int size)
{
	Matrix result;
	result.rows = size;
	result.cols = size;
	result.data = calloc(size*size, sizeof(int));

	return result;
}

void fillMatrix(Matrix* matrix)
{
	int index = 0;
	for (int row=0; row<matrix->rows; ++row)
	{
		for (int col=0; col<matrix->cols; ++col, ++index)
			matrix->data[index] = rand() % 3;
	}
}

void purge(Matrix* matrix)
{
	for (int row=0; row<matrix->rows; ++row)
	{
		for (int col=0; col<matrix->cols; ++col)
		{
			int index = row*matrix->cols + col;
			printf("%d ", matrix->data[index]);
			matrix->data[index] = 0;
		}

		printf ("\n");
	}

	printf ("\n");
}

MPI_Datatype createLine(int cols)
{
	MPI_Datatype result;
	MPI_Type_contiguous(cols, MPI_INT, &result);
	MPI_Type_commit(&result);

	return result;
}

int main(int argc, char** argv)
{
	if (argc!=2)
	{
		printf("Passer un entier en paramètre ; il sera multiplié par le nombre de processus pour déterminer le nombre de lignes et colonnes des matrices.\n");
		return 0;
	}

	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	int mult = atoi(argv[1]);
	int matRows = mult * world_size;
	Matrix left = createMatrix(matRows);
	Matrix right = createMatrix(matRows);
	MPI_Datatype Line = createLine(matRows);

	if (wrank==0)
	{
		srand(time(NULL));
		fillMatrix(&left);
		fillMatrix(&right);
	}

	// Regroupement des résultats.
	Matrix pResult;
	if (wrank==0)
		pResult = createMatrix(matRows);

	double start = MPI_Wtime();
	MPI_Bcast(left.data, left.rows, Line, 0, MPI_COMM_WORLD);
	MPI_Bcast(right.data, right.rows, Line, 0, MPI_COMM_WORLD);

	int* resultLine = calloc(mult*matRows, sizeof(int));
	int index = 0;
	for (int range=0; range<mult; ++range)
	{
		for (int col=0; col<left.cols; ++col, ++index)
		{
			int result = 0;
			for (int i=0; i<left.rows; ++i)
				result += left.data[(mult*wrank+range)*left.cols + i] * right.data[col + i*left.cols];

			resultLine[index] = result;
		}
	}

	MPI_Gather(resultLine, mult, Line, pResult.data, mult, Line, 0, MPI_COMM_WORLD);

	if (wrank==0)
	{
		printf("Parallèle : %f\n", MPI_Wtime()-start);

		Matrix result = createMatrix(matRows);
		start = MPI_Wtime();
		for (int row=0; row<left.rows; ++row)
		{
			for (int col=0; col<left.cols; ++col)
			{
				int one = 0;
				for (int i=0; i<left.rows; ++i)
					one += left.data[row*left.cols + i] * right.data[col + i*right.cols];

				result.data[row*result.cols + col] = one;
			}
		}

		printf("Série : %f\n", MPI_Wtime()-start);

		// Comparaison des résultats.
		int success = 1;
		for (int i=0; i<result.rows*result.cols; ++i)
		{
			if (pResult.data[i] != result.data[i])
			{
				success = 0;
				break;
			}
		}

		printf("Succès ? %d.\n", success);

		free(pResult.data);
		free(result.data);

		MPI_Type_free(&Line);
	}

	free(left.data);
	free(right.data);
	free(resultLine);

	MPI_Finalize();
	return 0;
}


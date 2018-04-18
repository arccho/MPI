#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Matrix_
{
	int rows;
	int cols;
	int* data;
} Matrix;

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

MPI_Datatype createCol(int rows, int cols)
{
	MPI_Datatype result;
	MPI_Type_vector(rows, 1, cols, MPI_INT, &result);
	MPI_Type_commit(&result);

	return result;
}

MPI_Datatype createDiag(int rows, int cols)
{
	int count = (rows < cols) ? rows : cols;
	int* blengths = calloc(count, sizeof(int));
	int* displs = calloc(count, sizeof(int));

	MPI_Datatype result;
	for (int i=0; i<count; ++i)
		blengths[i] = 1;

	displs[0] = 0;
	for (int i=1; i<count; ++i)
		displs[i] = displs[i-1] + (cols+1);

	MPI_Type_indexed(count, blengths, displs, MPI_INT, &result);
	MPI_Type_commit(&result);

	free(blengths);
	free(displs);

	return result;
}

MPI_Datatype createUpper(int rows, int cols)
{
	int count = (rows < cols) ? rows : cols;
	int* blengths = calloc(count, sizeof(int));
	int* displs = calloc(count, sizeof(int));

	MPI_Datatype result;
	blengths[0] = cols;
	for (int i=1; i<count; ++i)
		blengths[i] = blengths[i-1] - 1;

	displs[0] = 0;
	for (int i=1; i<count; ++i)
		displs[i] = displs[i-1] + (cols+1);

	MPI_Type_indexed(count, blengths, displs, MPI_INT, &result);
	MPI_Type_commit(&result);

	free(blengths);
	free(displs);

	return result;
}

void oneByOne(Matrix* pmatrix, int wrank)
{
	MPI_Datatype Line = createLine(pmatrix->cols);
	MPI_Datatype Col = createCol(pmatrix->rows, pmatrix->cols);
	MPI_Datatype Diag = createDiag(pmatrix->rows, pmatrix->cols);
	MPI_Datatype Upper = createUpper(pmatrix->rows, pmatrix->cols);

	if (wrank==0)
	{
		MPI_Ssend(pmatrix->data + 3*pmatrix->cols, 1, Line, 1, 0, MPI_COMM_WORLD);
		MPI_Ssend(pmatrix->data + 2, 1, Col, 1, 1, MPI_COMM_WORLD);
		MPI_Ssend(pmatrix->data, 1, Diag, 1, 2, MPI_COMM_WORLD);
		MPI_Ssend(pmatrix->data, 1, Upper, 1, 3, MPI_COMM_WORLD);
	}
	else if (wrank==1)
	{
		MPI_Recv(pmatrix->data + 3*pmatrix->cols, 1, Line, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Reçu une ligne.\n");
		purge(pmatrix);
		MPI_Recv(pmatrix->data + 2, 1, Col, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Reçu une colonne.\n");
		purge(pmatrix);
		MPI_Recv(pmatrix->data, 1, Diag, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Reçu la diagonale.\n");
		purge(pmatrix);
		MPI_Recv(pmatrix->data, 1, Upper, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Reçu la matrice triangulaire supérieure.\n");
		purge(pmatrix);
	}

	MPI_Type_free(&Line);
	MPI_Type_free(&Col);
	MPI_Type_free(&Diag);
	MPI_Type_free(&Upper);
}

void reorder(Matrix* pmatrix, int wrank)
{
	MPI_Datatype Line = createLine(pmatrix->cols);
	MPI_Datatype Col = createCol(pmatrix->rows, pmatrix->cols);

	if (wrank==0)
	{
		for (int row=0; row<pmatrix->rows; ++row)
			MPI_Ssend(pmatrix->data + row*pmatrix->cols, 1, Line, 1, row, MPI_COMM_WORLD);

		for (int col=0; col<pmatrix->cols; ++col)
			MPI_Ssend(pmatrix->data + col, 1, Col, 1, col, MPI_COMM_WORLD);
	}
	else if (wrank==1)
	{
		for (int row=0; row<pmatrix->rows; ++row)
			MPI_Recv(pmatrix->data + (pmatrix->rows-1-row)*pmatrix->cols, 1, Line, 0, row, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Lignes réordonnées.\n");
		purge(pmatrix);

		for (int col=0; col<pmatrix->cols; ++col)
			MPI_Recv(pmatrix->data + (pmatrix->cols-1-col), 1, Col, 0, col, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Colonnes réordonnées.\n");
		purge(pmatrix);
	}

	MPI_Type_free(&Line);
	MPI_Type_free(&Col);
}

void transpose(Matrix* pmatrix, int wrank)
{
	if (wrank==0)
	{
		MPI_Datatype Line = createLine(pmatrix->cols);

		for (int row=0; row<pmatrix->rows; ++row)
			MPI_Ssend(pmatrix->data + row*pmatrix->cols, 1, Line, 1, row, MPI_COMM_WORLD);

		MPI_Type_free(&Line);
	}
	else if (wrank==1)
	{
		pmatrix->rows = 4;
		pmatrix->cols = 5;

		MPI_Datatype Col = createCol(pmatrix->rows, pmatrix->cols);

		for (int col=0; col<pmatrix->cols; ++col)
			MPI_Recv(pmatrix->data + col, 1, Col, 0, col, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Transposée.\n");
		purge(pmatrix);

		MPI_Type_free(&Col);
	}
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int world_size, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	Matrix matrix;
	matrix.rows = 5;
	matrix.cols = 4;
	matrix.data = calloc(matrix.rows*matrix.cols, sizeof(int));

	if (wrank==0)
	{
		int index = 0;
		for (int row=0; row<matrix.rows; ++row)
		{
			for (int col=0; col<matrix.cols; ++col, ++index)
				matrix.data[index] = row*10 + col;
		}
	}
	else
	{
		for (int i=0; i<matrix.rows*matrix.cols; ++i)
			matrix.data[i] = 0;
	}

	oneByOne(&matrix, wrank);
	reorder(&matrix, wrank);
	transpose(&matrix, wrank);

	free(matrix.data);
	MPI_Finalize();
	return 0;
}


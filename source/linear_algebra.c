#include "linear_algebra.h"

void InvertMatrix(float *M, int n)
{
	float L[n][n];
	float U[n][n];

	LUFactorize(M, &L[0][0], &U[0][0], n);

	InvertLMatrix(&L[0][0], n);
	InvertUMatrix(&U[0][0], n);

	MultiplyULMatrix(&U[0][0], &L[0][0], M, n);
}

/* *
 * Factorizes a general matrix into a Upper triangular
 * and a Lower triangular matrix.
 * */
void LUFactorize(float *a, float *L, float *U, int n)
{
	int k, i, j;

	for(k = 0; k < n; k++)
	{
		L[k*n+k] = 1;
		for(i = k + 1; i < n; i++)
		{
			L[i*n+k]=a[i*n+k]/a[k*n+k];
				for(j = k + 1; j < n; j++)
				a[i*n+j] = a[i*n+j]-L[i*n+k]*a[k*n+j];
		}
		for(j = k; j < n; j++)
			U[k*n+j] = a[k*n+j];
	}
}

/* *
 * Inverts a Lower trianular matrix.
 * */
void InvertLMatrix(float *L, int n)
{
	float sum;
	int i,j,k;

	for (j = 0; j < n; j++)
	{
		for (i = j + 1; i < n; i++)
		{
			sum = 0.0f;

			for (k = j; k < i; k++)
				sum -= L[i*n+k]*L[k*n+j];

			L[i*n+j] = sum;
		}
	}
}

/* *
 * Inverts a Upper trianular matrix.
 * */
void InvertUMatrix(float *U, int n)
{
	float sum;
	int i,j,k;

	for (j = n - 1; j > -1; j--)
		U[j*n+j] = 1.0f/U[j*n+j];

	for (j = n - 1; j > -1; j--)
	{
		for (i = j - 1; i > -1; i--)
		{
			sum = 0.0f;

			for (k = j; k > i; k--)
				sum -= U[i*n+k]*U[k*n+j];

			U[i*n+j] = sum*U[i*n+i];
		}
	}
}

/* *
 * Multiplies two gerneral matrices.
 * */
void MatrixMultiply(float *a, int aRows, int aColumns, float *b, int bColumns, float *resultant)
{
	float sum;

	for(int i = 0; i < aRows; i++)
		for(int j = 0; j < bColumns; j++)
		{
			sum = 0.0f;

			for(int k = 0; k < aColumns; k++)
				sum += a[i*aColumns+k] * b[k*bColumns+j];

			resultant[i*bColumns+j] = sum;
		}
}

/* *
 * Multiplies a Upper triangular matrix with a Lower triangular matrix
 * */
void MultiplyULMatrix(float *a, float *b, float *resultant, int n)
{
	/* *
	 *
	 * Illustration why we don't have to multiply and add every element:
	 * x is some number in the matrix, but the x:es are not necessarily the same.
	 *
	 * | x x x x x |   | x 0 0 0 0 |
	 * | 0 x x x x |   | x x 0 0 0 |
	 * | 0 0 x x x | * | x x x 0 0 | = ...
	 * | 0 0 0 x x |   | x x x x 0 |
	 * | 0 0 0 0 x |   | x x x x x |
	 *
	 * Many zeroes that don't have to be taken into account when multiplying!
	 * Also if the 0-position data is not zeroed does not matter.
	 *
	 * */
	float sum;

	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			sum = 0.0f;
			int k;

			if (j < i)
				k = i;
			else
				k = j;

			for(; k < n; k++)
				sum += a[i*n+k] * b[k*n+j];

			resultant[i*n+j] = sum;
		}
	}
}

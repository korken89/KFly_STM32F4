#ifndef __LINEAR_ALGEBRA_H
#define __LINEAR_ALGEBRA_H

/* Standard includes */
#include <math.h>

/* Driver includes */

/* Scheduler includes. */

/* KFly includes */

/* Includes */

/* Defines */
#define SQR(a) ((a) * (a))
#define SIGN(a, b) ((b) >= 0.0f ? fabsf(a) : -fabsf(a))
#define A(i, j) (a[(i)*(n) + (j)])
#define As(i, j, m) (a[(i)*(m) + (j)])
#define B(i, j) (b[(i)*n + (j)])
#define Bs(i, j, m) (b[(i)*(m) + (j)])
#define X(i) (x[(i)])

/* Typedefs */

/* Global variable defines */

/* Inline function defines */

/*
 * givens - Creates a Givens rotation to zero the lower
 *			of two elements in a matrix/vector.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in] a			Input to the first element. 
 * @param[in] b			Input to the second element. 
 * @param[out] c		Cosine output
 * @param[out] s		Sine output
 */
static inline void givens(float a, float b, float *c, float *s)
{
	float r;

	if (b == 0)
	{
		*c = 1.0f;
		*s = 0.0f;
	}
	else
	{
		r = 1.0f / sqrtf(a*a + b*b);
		*c = a * r;
		*s = b * r;
	}
}

/*
 * givens_mapp - Applies a Givens rotation to a matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element. 
 * @param[in] c			Cosine input
 * @param[in] s			Sine input
 * @param[in] i			Input to the i-th column/row.
 * @param[in] j			Input to the j-th column/row.
 * @param[in] n			Columns in matrix
 */
static inline void givens_mapp(float *a, float c, float s, const int i, const int j, const int n)
{
	int k;
	float tmp;

	for (k = 0; k < n; k++)
	{
		tmp = c * A(i, k) + s * A(j, k);
		A(j, k) = -s * A(i, k) + c * A(j, k);
		A(i, k) = tmp;
	}
}

/*
 * givens_vapp - Applies a Givens rotation to a vector and
 *				 returns the cosine and sine components.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] x		Input/output vector. 
 * @param[out] c		Cosine output
 * @param[out] s		Sine output
 * @param[in] i			Input to the i-th column/row.
 * @param[in] j			Input to the j-th column/row.
 * @param[in] n			Columns in matrix
 */
static inline void givens_vapp(float *x, float *c, float *s, const int i, const int j)
{
	givens(X(i), X(j), c, s);

	X(i) = (*c) * X(i) + (*s) * X(j);
	X(j) = 0.0f;
}

/*
 * fwd_sub - Solves the linear system L*x = b using forward substitution.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in] a			Input matrix, a points to the first element. 
 * @param[in/out] b		Input/output matrix, b points to the first element. 
 * @param[in] m			Number of rows in matrix.
 * @param[in] n			Number of columns in matrix, L is assumed m x m.
 */
static inline void fwd_sub(const float *a, float *b, const int m, const int n)
{
	int i, j, k;
	float sum;

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
		{
			for (sum = Bs(j, i, n), k = 0; k < j; k++)
				sum -= As(j, k, m) * Bs(k, i, n);

			Bs(j, i, n) = sum / As(j, j, m);
		}
}

/*
 * bck_sub - Solves the linear system U*x = b using backward substitution.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in] a			Input matrix, a points to the first element. 
 * @param[in/out] b		Input/output matrix, b points to the first element. 
 * @param[in] m			Number of rows in matrix.
 * @param[in] n			Number of columns in matrix, U is assumed m x m.
 */
static inline void bck_sub(const float *a, float *b, const int m, const int n)
{
	int i, j, k;
	float sum;

	for (i = 0; i < n; i++)
		for (j = m - 1; j >= 0; j--)
		{
			for (sum = Bs(j, i, n), k = j + 1; k < m; k++)
				sum -= As(j, k, m) * Bs(k, i, n);

			Bs(j, i, n) = sum / As(j, j, m);
		}
}

/*
 * qr_decomp - QR Decomposition of skinny (tall) matrices where m >= n,
 *			   returns the upper triangular matrix R in the top of A.
 *			   NOTE: This algorithm uses solving householder matrices.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element. 
 * @param[in] m			Number of rows in matrix.
 * @param[in] n			Number of columns in matrix.
 */
static inline void qr_decomp(float *a, const int m, const int n)
{
	int i, j, k;
	float norm, sigma, sum, tau, u1;

	// Loop over every column
	for (k = 0; k < n; k++)
	{
		// Calculate the norm of the vector
		for (j = k, sum = 0.0f; j < m; j++)
			sum += A(j, k) * A(j, k);

		norm = sqrtf(sum);
		sigma = SIGN(norm, A(k, k));
		u1 = A(k, k) + sigma;

		// Find Hk and Hk * A

		for (j = k + 1; j < m; j++)
			A(j, k) /= u1;

		A(k, k) = -sigma;
		tau = u1 / sigma;

		for (j = k + 1; j < n; j++) 
		{
			for (sum = A(k, j), i = k + 1; i < m; i++)
				sum += A(i, k) * A(i, j);

			A(k, j) -= tau * sum;

			for (i = k + 1; i < m; i++)
				A(i, j) -= tau * sum * A(i, k);
		}
	}
}

/*
 * qr_decomp_tria - Optimized QR Decomposition of matrices where m = 2n
 *					and where the last n rows is an triangular matrix. Returns
 *					the upper triangular matrix R in the top of A.
 *					NOTE: This algorithm uses solving householder matrices.
 *
 *					Example input matrix with n = 2:
 *				          | x  x |
 *				      A = | x  x |
 *				          | x  x |
 *				          | 0  x |
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element. 
 * @param[in] n			Number of columns in matrix.
 */
static inline void qr_decomp_tria(float *a, const int n)
{
	int i, j, k;
	float norm, sigma, sum, tau, u1, u2;

	// Loop over every column
	for (k = 0; k < n; k++)
	{
		// Calculate the norm of the vector
		for (j = k, sum = 0.0f; j < k + n + 1; j++)
			sum += A(j, k) * A(j, k);

		norm = sqrtf(sum);
		sigma = SIGN(norm, A(k, k));
		u1 = A(k, k) + sigma;

		// Find Hk and Hk * A

		u2 = 1.0f / u1;

		for (j = k + 1; j < k + n + 1; j++)
			A(j, k) *= u2;

		A(k, k) = -sigma;
		tau = u1 / sigma;

		for (j = k + 1; j < n; j++) 
		{
			for (sum = A(k, j), i = k + 1; i < k + n + 1; i++)
				sum += A(i, k) * A(i, j);

			A(k, j) -= tau * sum;

			for (i = k + 1; i < k + n + 1; i++)
				A(i, j) -= tau * sum * A(i, k);
		}
	}
}

/*
 * qr_downdate - Downdates a QR factorization on the formula: Anew = A - x*x'
 *				 uising Givens rotations. Math from "A note on downdating the 
 *				 Cholesky factorization" 
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element. 
 * @param[in/out] x		Input/output vector, a points to the first element. 
 * @param[in] n			Number of columns in matrix.
 */
static inline void qr_downdate(float *a, float *x, const int n)
{

}

/*
 * chol_decomp_lower - Cholesky Decomposition (matrix square-root) of square
 *				 	   matrices. Returns the lower triangular matrix L in the
 *				 	   bottom of A so that L*L' = A.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void chol_decomp_lower(float *a, const int n)
{
	int i, j, k;
	float sum;

	for (j = 0; j < n; j++)
	{
		for (sum = A(j, j), k = 0; k < j; k++)
			sum -= A(j, k) * A(j, k);

		A(j, j) = sqrtf(sum);

		for (i = j + 1; i < n; i++)
		{
			for (sum = A(i, j), k = 0; k < j; k++)
				sum -= A(i, k) * A(j, k);

			A(i, j) = sum / A(j, j);
		}
	}
}

/*
 * chol_decomp_upper - Cholesky Decomposition (matrix square-root) of square
 *				 	   matrices. Returns the upper triangular matrix U in the
 *				 	   top of A so that U'*U = A.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void chol_decomp_upper(float *a, const int n)
{
	int i, j, k;
	float sum;

	for (j = 0; j < n; j++)
	{
		for (sum = A(j, j), k = 0; k < j; k++)
			sum -= A(k, j) * A(k, j);

		A(j, j) = sqrtf(sum);

		for (i = j + 1; i < n; i++)
		{
			for (sum = A(j, i), k = 0; k < j; k++)
				sum -= A(k, i) * A(k, j);

			A(j, i) = sum / A(j, j);
		}
	}
}

/*
 * chol_update - Updates a Cholesky Decomposition, provided in a, with 
 *				 the vector x.
 *				 Anew = A + x*x' where A = L*L' it is possible to directly
 *				 update the decomposition L to Lnew.
 *		http://en.wikipedia.org/wiki/Cholesky_decomposition#Rank-one_update
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] x			Input vector for the update.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void chol_update(float *a, float *x, const int n)
{
	int i, j;
	float r, c, cinv, s, tmp;

	for (i = 0; i < n-1; i++)
	{
		tmp = A(i, i);
		r = sqrtf(tmp * tmp + X(i) * X(i));
		tmp = 1 / tmp;
		c = r * tmp;
		cinv = 1 / c;
		s = X(i) * tmp;

		A(i, i) = r;

		for (j = i + 1; j < n; j++)
		{
			A(i, j) = ( A(i, j) + s * X(j) ) * cinv;
			X(j) = c * X(j) - s * A(i, j);
		}
	}

	tmp = A(i, i);
	r = sqrtf(tmp * tmp + X(i) * X(i));
	A(i, i) = r;
}

/*
 * chol_downdate - Updates a Cholesky Decomposition, provided in a, with 
 *				   the vector x.
 *				   Anew = A - x*x' where A = L*L' it is possible to directly
 *				   update the decomposition L to Lnew.
 *		http://en.wikipedia.org/wiki/Cholesky_decomposition#Rank-one_update
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] x			Input vector for the update.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void chol_downdate(float *a, float *x, const int n)
{
	int i, j;
	float r, c, cinv, s, tmp;

	for (i = 0; i < n-1; i++)
	{
		tmp = A(i, i);
		r = sqrtf(tmp * tmp - X(i) * X(i));
		tmp = 1 / tmp;
		c = r * tmp;
		cinv = 1 / c;
		s = X(i) * tmp;

		A(i, i) = r;

		for (j = i + 1; j < n; j++)
		{
			A(i, j) = ( A(i, j) - s * X(j) ) * cinv;
			X(j) = c * X(j) - s * A(i, j);
		}
	}

	tmp = A(i, i);
	r = sqrtf(tmp * tmp - X(i) * X(i));
	A(i, i) = r;
}

/*
 * lu_decomp - LU Decomposition of square matrices.
 *			   Returns the lower triangular matrix L in the bottom
 *			   of A and the upper triangular matrix u in the top of
 *			   A so that (in an 3x3 example):
 *
 *				| U_11 U_12 U_12 |
 *		LU(A) = | L_21 U_22 U_23 | where diag(L) = [1, 1, 1]
 *				| L_31 L_32 U_33 |
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void lu_decomp(float *a, const int n)
{
	int k, i, j;
	float tmp1, tmp2;

	for(k = 0; k < n; k++)
	{
		tmp2 = 1.0f / A(k, k);

		for(i = k + 1; i < n; i++)
		{
			tmp1 = A(i, k) = A(i, k) * tmp2;

			for(j = k + 1; j < n; j++)
				A(i, j) -= tmp1 * A(k, j);
		}
	}
}

/*
 * u_inv - Inverts a general upper triangular matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void u_inv(float *a, const int n)
{
	int i,j,k;
	float sum;

	for (j = n - 1; j > -1; j--)
		A(j, j) = 1.0f / A(j, j);

	for (j = n - 1; j > -1; j--)
	{
		for (i = j - 1; i > -1; i--)
		{
			for (sum = 0.0f, k = j; k > i; k--)
				sum -= A(i, k) * A(k, j);

			A(i, j) = sum * A(i, i);
		}
	}
}

/*
 * l_inv - Inverts a general lower triangular matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void l_inv(float *a, const int n)
{
	int i, j, k;
	float sum;

	for (j = 0; j < n; j++)
		A(j, j) = 1.0f / A(j, j);

	for (j = 0; j < n; j++)
	{
		for (i = j + 1; i < n; i++)
		{
			for (sum = 0.0f, k = j; k < i; k++)
				sum -= A(i, k) * A(k, j);

			A(i, j) = sum * A(i, i);
		}
	}
}

/*
 * unit_l_inv - Inverts a lower triangular matrix with all ones in the diagonal.
 *				Specially made for the inverse from the result of lu_decomp().
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void unit_l_inv(float *a, const int n)
{
	int i, j, k;
	float sum;

	for (j = 0; j < n; j++)
	{
		for (i = j + 1; i < n; i++)
		{
			for (sum = -A(i, j), k = j + 1; k < i; k++)
				sum -= A(i, k) * A(k, j);

			A(i, j) = sum;
		}
	}
}

/*
 * ul_mul - Multiplies an upper triangular matrix with a lower triangular matrix.
 *			Specially made for the inverse from the result of lu_decomp().
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void ul_mul(float *a, const int n)
{
	/* *
	 *
	 * Illustration why we don't have to multiply and add every element:
	 * x is some number in the matrix, but the x:es are not generally the same.
	 *
	 * | x x x x x |   | 1 0 0 0 0 |
	 * | 0 x x x x |   | x 1 0 0 0 |
	 * | 0 0 x x x | * | x x 1 0 0 | = ...
	 * | 0 0 0 x x |   | x x x 1 0 |
	 * | 0 0 0 0 x |   | x x x x 1 |
	 *
	 * Many zeroes and ones that simplifies when multiplying!
	 * Also if the 0-position data is not zeroed does not matter.
	 *
	 * */

	int i, j, k;
	float sum;

	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n-1; j++) /* There is no need to calculate
								    the last column: U(:, n) * 1 */
		{
			if (j < i)
			{
				k = i-1;
				sum = 0.0f;
			}
			else
			{
				k = j;
				sum = A(i, j);
			}
			
			for(k = k + 1; k < n; k++)
				sum += A(i, k) * A(k, j);

			A(i, j) = sum;
		}
	}
}

/*
 * uu_mul - Multiplies an upper triangular matrix with another upper triangular matrix.
 *			Returns the result of the product in the second matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in] a			Input matrix, a points to the first element. 
 * @param[in/out] b		Input/output matrix, b points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void uu_mul(float *a, float *b, const int n)
{
	/* *
	 *
	 * Illustration why we don't have to multiply and add every element:
	 * x is some number in the matrix, but the x:es are not generally the same.
	 *		 A 		 *		 B 		 = B_new
	 * | x x x x x |   | x x x x x |
	 * | 0 x x x x |   | 0 x x x x |
	 * | 0 0 x x x | * | 0 0 x x x | = ...
	 * | 0 0 0 x x |   | 0 0 0 x x |
	 * | 0 0 0 0 x |   | 0 0 0 0 x |
	 *
	 * Many zeroes and ones that simplifies when multiplying!
	 * Also if the 0-position data is not zeroed does not matter.
	 *
	 * */

	int i, j, k;
	float sum;

	for (i = 0; i < n; i++)
	{
		for (j = i; j < n; j++)
		{
			sum = 0.0f;

			for (k = i; k < j+1; k++)	
				sum += A(i, k) * B(k, j);

			B(i, j) = sum;
		}
	}
}

/*
 * general_inv - Inverts a general nxn matrix using LU Decomposition.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void general_inv(float *a, const int n)
{
	/* Decompose matrix into LU form */
	lu_decomp(a, n);

	/* Invert U and L */
	u_inv(a, n);
	unit_l_inv(a, n);

	/* Multiply a = U^-1 * L^-1 */
	ul_mul(a, n);
}

/*
 * create_identity - Creates an identity matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void create_identity(float *a, const int n)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
		A(i, i) = 1.0f;

		for (j = i+1; j < n; j++)
		{
			A(i, j) = 0.0f;
			A(j, i) = 0.0f;
		}
	}
}

/*
 * create_identity_tria - Creates an upper triangular identity matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows and columns in matrix.
 */
static inline void create_identity_tria(float *a, const int n)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
		A(i, i) = 1.0f;

		for (j = i+1; j < n; j++)
			A(i, j) = 0.0f;
	}
}

/*
 * create_zero - Creates an zero matrix.
 *
 * @author				Emil Fresk, Luleå University of Technology
 * @param[in/out] a		Input/output matrix, a points to the first element.  
 * @param[in] n			Number of rows in matrix.
 * @param[in] m			Number of columns in matrix.
 */
static inline void create_zero(float *a, const int n, const int m)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			As(i, j, m) = 0.0f;
		}
	}
}


/* Global function defines */

#endif

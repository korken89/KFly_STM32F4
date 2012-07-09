#ifndef __LINEAR_ALGEBRA_H
#define __LINEAR_ALGEBRA_H

/* Standard includes */

/* Driver includes */

/* Scheduler includes. */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* These are all the commands for the serial protocol. */

/* Length of commands */

/* Global variable defines */

/* Global function defines */
void InvertMatrix(float *, int);
void LUFactorize(float *, float *, float *, int);
void InvertLMatrix(float *, int);
void InvertUMatrix(float *, int);
void MatrixMultiply(float *, int, int, float *, int, float *);
void MultiplyULMatrix(float *, float *, float *, int);

#endif

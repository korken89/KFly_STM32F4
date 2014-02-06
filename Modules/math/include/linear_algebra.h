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
void InvertMatrix(float *);
void LUFactorize(float *, float *, float *);
void InvertLMatrix(float *);
void InvertUMatrix(float *);
void MatrixMultiply(float *, int, int, float *, int, float *);
void MultiplyULMatrix(float *, float *, float *);

#endif

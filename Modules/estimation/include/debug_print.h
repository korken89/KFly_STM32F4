#ifndef __DEBUG_PRINT_H
#define __DEBUG_PRINT_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"
#include "comlink.h"
#include "linear_algebra.h"

void print_float(float f);
void print_vector(vector3f_t *v);
void print_quaternion(quaternion_t *q);
void print_matrix(void *a, uint32_t r, uint32_t c);

#endif

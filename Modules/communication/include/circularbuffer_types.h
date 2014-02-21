#ifndef __CIRCULARBUFFER_TYPES_H
#define __CIRCULARBUFFER_TYPES_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_dma.h"
#include "statemachine_types.h"
#include "uart.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef struct
{
	xSemaphoreHandle write_lock;	/* Write lock mutex */
	uint32_t head;					/* Newest element */
	uint32_t tail;					/* Oldest element */
	uint32_t size;					/* Size of buffer */
	uint8_t *buffer;				/* Pointer to memory area */
} Circular_Buffer_Type;

/* Global variable defines */

/* Global function defines */


#endif

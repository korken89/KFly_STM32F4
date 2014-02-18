#ifndef __CIRCULARBUFFER_H
#define __CIRCULARBUFFER_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_dma.h"

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
	uint32_t count;					/* Number of elements in buffer */
	uint32_t start;					/* Oldest element */
	uint8_t *buffer;				/* Pointer to memory area */
} Circular_Buffer_Type;

/* Global variable defines */

/* Global function defines */

/* Inline functions */
static inline void CircularBuffer_Init(Circular_Buffer_Type *Cbuff, uint8_t *buffer)
{
	Cbuff->count = 0;
	Cbuff->start = 0;
	Cbuff->buffer = buffer;
}

static inline void CircularBuffer_InitMutex(Circular_Buffer_Type *Cbuff)
{
	Cbuff->write_lock  = xSemaphoreCreateMutex();
}

static inline portBASE_TYPE CircularBuffer_Claim(Circular_Buffer_Type *Cbuff, portTickType timeout)
{
	return xSemaphoreTake(Cbuff->write_lock, timeout);
}

static inline portBASE_TYPE CircularBuffer_Release(Circular_Buffer_Type *Cbuff)
{
	return xSemaphoreGive(Cbuff->write_lock);
}

static inline void CircularBuffer_ReleaseFromISR(Circular_Buffer_Type *Cbuff)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(Cbuff->write_lock, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken != pdFALSE)
		vPortYieldFromISR();
}


static inline uint32_t CircularBuffer_SpaceLeft(Circular_Buffer_Type *Cbuff, const uint32_t buffer_size)
{
	return  (buffer_size - Cbuff->count);
}

static inline void CircularBuffer_WriteSingle(Circular_Buffer_Type *Cbuff, uint8_t data, const uint32_t buffer_size)
{
	Cbuff->buffer[Cbuff->start + Cbuff->count] = data;
	Cbuff->count = (Cbuff->count + 1) % buffer_size;
}

/* *
 *
 * This algorithm assumes you have checked that the data will fit inside the buffer.
 *
 * */
static inline void CircularBuffer_WriteChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, const uint32_t data_size, const uint32_t buffer_size)
{
	uint32_t i, head, from_bot, to_top;

	head = (Cbuff->start + Cbuff->count) % buffer_size;
	to_top = buffer_size - head;

	if (to_top < data_size)
	{	/* If we need to wrap around during the write */
		from_bot = data_size - to_top;

		/* First we fill to the top */
		for (i = 0; i < to_top; i++)
			Cbuff->buffer[head + i] = data[i];

		/* The we fill the rest */
		for (i = 0; i < from_bot; i++)
			Cbuff->buffer[i] = data[to_top + i];
	}
	else
	{	/* No wrap around needed, chunk will fit in the space left to the top */
		for (i = 0; i < data_size; i++)
			Cbuff->buffer[head + i] = data[i];
	}

	Cbuff->count += data_size;
}

static inline void CircularBuffer_ReadSingle(Circular_Buffer_Type *Cbuff, uint8_t *data, const uint32_t buffer_size)
{
	*data = Cbuff->buffer[Cbuff->start];
	Cbuff->start = (Cbuff->start + 1) % buffer_size;
	Cbuff->count -= 1;
}

static inline void CircularBuffer_ReadChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, uint32_t count, const uint32_t buffer_size)
{

}

#endif

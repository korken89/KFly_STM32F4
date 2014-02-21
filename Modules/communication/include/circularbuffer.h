#ifndef __CIRCULARBUFFER_H
#define __CIRCULARBUFFER_H

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

/* Inline functions */
static inline void CircularBuffer_Init(Circular_Buffer_Type *Cbuff, uint8_t *buffer, uint32_t buffer_size)
{
	Cbuff->head = 0;
	Cbuff->tail = 0;
	Cbuff->size = buffer_size;
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


static inline uint32_t CircularBuffer_SpaceLeft(Circular_Buffer_Type *Cbuff)
{
	return  ((Cbuff->head - Cbuff->tail + Cbuff->size) % Cbuff->size);
}

static inline void CircularBuffer_WriteSingle(Circular_Buffer_Type *Cbuff, uint8_t data)
{
	Cbuff->buffer[Cbuff->head] = data;
	Cbuff->head = (Cbuff->head + 1) % Cbuff->size;
}

/* *
 *
 * This algorithm assumes you have checked that the data will fit inside the buffer.
 *
 * */
static inline void CircularBuffer_WriteChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, const uint32_t count)
{
	uint32_t i, head, from_bot, to_top;

	head = Cbuff->head;
	to_top = Cbuff->size - Cbuff->head;


	if (to_top < count)
	{	/* If we need to wrap around during the write */
		from_bot = count - to_top;

		/* First we fill to the top */
		for (i = 0; i < to_top; i++)
			Cbuff->buffer[head + i] = data[i];

		/* Then we fill the rest */
		for (i = 0; i < from_bot; i++)
			Cbuff->buffer[i] = data[to_top + i];

		Cbuff->head = from_bot; /* The end value of head was pre-calculated */
	}
	else
	{	/* No wrap around needed, chunk will fit in the space left to the top */
		for (i = 0; i < count; i++)
			Cbuff->buffer[head + i] = data[i];

		Cbuff->head = (Cbuff->head + count); /* There will be no wrap around */
	}
}

static inline void CircularBuffer_WriteSYNCNoIncrement(Circular_Buffer_Type *Cbuff, int *count, uint8_t *crc8, uint16_t *crc16)
{
	/* Check if we have 1 byte free for SYNC */
	if (CircularBuffer_SpaceLeft(Cbuff) >= 1)
	{
		Cbuff->buffer[(Cbuff->head + *count) % Cbuff->size] = SYNC_BYTE;
		*count += 1;

		/* When writing the SYNC CRC8 must be calculated */
		*crc8 = CRC8_step(SYNC_BYTE, 0x00);

		if (crc16 != NULL)
			*crc16 = CRC16_step(SYNC_BYTE, 0xffff);	
	}
	else
		*count = -1;
}

static inline void CircularBuffer_WriteNoIncrement(uint8_t data, Circular_Buffer_Type *Cbuff, int *count, uint8_t *crc8, uint16_t *crc16)
{
	/* Check if we have an error from previous write */
	if (count >= 0)
	{
		/* Check if we have 2 bytes free, in case of data = SYNC */
		if (CircularBuffer_SpaceLeft(Cbuff) >= 2)
		{
			Cbuff->buffer[(Cbuff->head + *count) % Cbuff->size] = data;
			*count += 1;

			if (crc8 != NULL)
				*crc8 = CRC8_step(data, *crc8);

			if (crc16 != NULL)
				*crc16 = CRC16_step(data, *crc16);	

			if (data == SYNC_BYTE)
			{
				Cbuff->buffer[(Cbuff->head + *count) % Cbuff->size] = SYNC_BYTE;
				*count += 1;
			}
		}
		else
			*count = -1;
	}
}

static inline ErrorStatus CircularBuffer_Increment(uint32_t count, Circular_Buffer_Type *Cbuff)
{
	if (count == -1) /* Error! */
		return ERROR;

	else
	{
		Cbuff->head = ((Cbuff->head + count) % Cbuff->size);
		return SUCCESS;
	}
}

static inline void CircularBuffer_ReadSingle(Circular_Buffer_Type *Cbuff, uint8_t *data)
{
	*data = Cbuff->buffer[Cbuff->tail];
	Cbuff->tail = ((Cbuff->tail + 1) % Cbuff->size);
}

static inline void CircularBuffer_ReadChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, uint32_t count)
{

}

static inline void CircularBuffer_DMATransmit(DMA_Stream_TypeDef *DMAx_Streamy, Circular_Buffer_Type *Cbuff)
{
	uint32_t count;

	if (Cbuff->head > Cbuff->tail)
	{	/* No wrap around */
		count = Cbuff->head - Cbuff->tail;
		DMA_Transmit_Buffer(DMAx_Streamy, &(Cbuff->buffer[Cbuff->tail]), count);

		/* Set tail to equal head since we are now at the same position */
		Cbuff->tail = ((Cbuff->tail + count) % Cbuff->size);
	}
	else
	{	/* Wrap around will occur. Transmit the data to the end of the buffer. */
		DMA_Transmit_Buffer(DMAx_Streamy, &(Cbuff->buffer[Cbuff->tail]), Cbuff->size - Cbuff->tail);

		/* Set tail to zero since we are now at the start of the buffer */
		Cbuff->tail = 0;
	}
}


#endif

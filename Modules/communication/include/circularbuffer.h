#ifndef __CIRCULARBUFFER_H
#define __CIRCULARBUFFER_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_dma.h"
#include "statemachine_types.h"
#include "uart.h"
#include "crc.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */
#include "circularbuffer_types.h"

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */

void CircularBuffer_Init(Circular_Buffer_Type *Cbuff, uint8_t *buffer, uint32_t buffer_size);
void CircularBuffer_InitMutex(Circular_Buffer_Type *Cbuff);
portBASE_TYPE CircularBuffer_Claim(Circular_Buffer_Type *Cbuff, portTickType timeout);
portBASE_TYPE CircularBuffer_Release(Circular_Buffer_Type *Cbuff);
void CircularBuffer_ReleaseFromISR(Circular_Buffer_Type *Cbuff);
uint32_t CircularBuffer_SpaceLeft(Circular_Buffer_Type *Cbuff);
void CircularBuffer_WriteSingle(Circular_Buffer_Type *Cbuff, uint8_t data);
void CircularBuffer_WriteChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, const uint32_t count);
void CircularBuffer_WriteSYNCNoIncrement(Circular_Buffer_Type *Cbuff, int *count, uint8_t *crc8, uint16_t *crc16);
void CircularBuffer_WriteNoIncrement(uint8_t data, Circular_Buffer_Type *Cbuff, int *count, uint8_t *crc8, uint16_t *crc16);
ErrorStatus CircularBuffer_Increment(uint32_t count, Circular_Buffer_Type *Cbuff);
void CircularBuffer_ReadSingle(Circular_Buffer_Type *Cbuff, uint8_t *data);
void CircularBuffer_ReadChunk(Circular_Buffer_Type *Cbuff, uint8_t *data, uint32_t count);
void CircularBuffer_DMATransmit(DMA_Stream_TypeDef *DMAx_Streamy, Circular_Buffer_Type *Cbuff);

/* Inline functions */

#endif

/******************************************************************************
 * @file :   	fifo.c
 * @brief : 	FIFO Functions
 * @version : 	0.1
 * @author :	Emil Fresk
 ******************************************************************************/

#include "fifo.h"

/**********************************************
 *
 * Software FIFO for use of different applications.
 * OBSERVE! It is only allowed to use the FIFO via these
 * functions, manual change is prohibbited and unpredictable.
 *
 * Supports up to 64kB FIFOs.
 *
 **********************************************/


ErrorStatus fifo_write(FIFO_TypeDef *fifoblock, uint8_t *data, uint16_t size)
{
	for (uint16_t i = 0; i <= size; i++)
	{
		*(fifoblock->base_addr + fifoblock->write_offset++) = *(data + i);

		if (fifoblock->write_offset >= fifoblock->size)
			fifoblock->write_offset = 0;
	}

	return SUCCESS;
}

ErrorStatus fifo_read(FIFO_TypeDef *fifoblock, uint8_t *data)
{
	if (fifoblock->read_offset == fifoblock->write_offset)	// When the FIFO is empty, return error.
		return ERROR;

	*data = *(fifoblock->base_addr + fifoblock->read_offset++);

	if (fifoblock->read_offset >= fifoblock->size)
		fifoblock->read_offset = 0;

	return SUCCESS;
}

ErrorStatus fifo_flush(FIFO_TypeDef *fifoblock)
{
	fifoblock->read_offset = 0;
	fifoblock->write_offset = 0;

	return SUCCESS;
}

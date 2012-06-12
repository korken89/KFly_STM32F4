#ifndef __FIFO_H
#define __FIFO_H

#include "stm32f4xx.h"

typedef struct
{
	uint8_t *base_addr;
	uint16_t read_offset;
	uint16_t write_offset;
	uint16_t size;
} FIFO_TypeDef;

ErrorStatus fifo_write(FIFO_TypeDef *, uint8_t *);
ErrorStatus fifo_read(FIFO_TypeDef *, uint8_t *);
ErrorStatus fifo_flush(FIFO_TypeDef *);

#endif

#ifndef __SERIALMANAGER_H
#define __SERIALMANAGER_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "comlink.h"
#include "crc.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define tskSerialManagerPRIORITY		(tskIDLE_PRIORITY + 1)

/* Typedefs */
typedef enum
{	/* This will help the parser function to identiy the Port receving data */
	PORT_USB = 0,
	PORT_AUX1,
	PORT_AUX2,
	PORT_AUX3,
	PORT_CAN
} RECEIVER_SOURCE_Type;

/* Global variable defines */

/* Global function defines */
void vInitSerialManager(void);
void vTaskUSBSerialManager(void *);
void vDataStreamParser(uint8_t, RECEIVER_SOURCE_Type);

#endif

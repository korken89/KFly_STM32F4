#ifndef __SERIALMANAGER_H
#define __SERIALMANAGER_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "statemachine.h"
#include "comlink.h"
#include "crc.h"
#include "circularbuffer.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define tskSerialManagerPRIORITY		(tskIDLE_PRIORITY + 1)

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void vSerialManagerInit(void);
void vTaskUSBSerialManager(void *);
Circular_Buffer_Type *GetCircularBufferFromAUXPort(Port_Type aux_port);

#endif

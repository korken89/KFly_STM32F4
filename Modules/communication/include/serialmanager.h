#ifndef __SERIALMANAGER_H
#define __SERIALMANAGER_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "statemachine.h"
#include "comlink.h"
#include "crc.h"
#include "serialmanager_parsers.h"
#include "serialmanager_types.h"

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


#endif

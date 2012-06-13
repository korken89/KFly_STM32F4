#ifndef __COMLINK_H
#define __COMLINK_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "usbd_cdc.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define USBQueueSize	256

/* Global variable defines */
extern volatile xQueueHandle xUSBQueueHandle;

/* Global function defines */
void vUSBQueueInit(void);
ErrorStatus xUSBSendData(uint8_t *, uint32_t);

#endif

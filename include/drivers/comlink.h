#ifndef __COMLINK_H
#define __COMLINK_H

/* Standard includes */
#include "stm32f4xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define USBQueueSize		256

/* Global variable defines */
extern volatile xQueueHandle xUSBQueue;

/* Global function defines */

/* *
 * TODO:
 * Temporary send function. Will in the future check if the USB is avalible.
 * */
#define xUSBSendData(data, size) cdc_DataTx(data, size)
void vUSBQueueInit(void);

#endif

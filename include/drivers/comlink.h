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
#include "serialmanager.h"

/* Defines */
#define xUSBQueueSize		256
#define AUX1 				USART3
#define AUX2 				USART1
#define AUX3 				UART4

/* Typedefs */
typedef struct
{
	volatile xQueueHandle xUSBQueueHandle;
	volatile Bool bUSBAvalible;
} USB_QUEUE_Type;

/* Global variable defines */
extern volatile USB_QUEUE_Type xUSBQueue;

/* Global function defines */
void vUSBQueueInit(void);
ErrorStatus xUSBSendData(uint8_t *, uint32_t);

#endif

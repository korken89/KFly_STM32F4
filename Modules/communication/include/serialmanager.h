#ifndef __SERIALMANAGER_H
#define __SERIALMANAGER_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
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

/* These are all the commands for the serial protocol. */

/* Length of commands */

/* Global variable defines */

/* Global function defines */
void vSerialManagerInit(void);
void vTaskUSBSerialManager(void *);
void vStatemachineDataEntry(uint8_t, Parser_Holder_Type *);
void vWaitingForSYNC(uint8_t, Parser_Holder_Type *);
void vWaitingForSYNCorCMD(uint8_t, Parser_Holder_Type *);
void vRxCmd(uint8_t, Parser_Holder_Type *);
void vRxSize(uint8_t, Parser_Holder_Type *);
void vRxCRC8(uint8_t, Parser_Holder_Type *);
void vRxData(uint8_t, Parser_Holder_Type *);
void vRxCRC16(uint8_t, Parser_Holder_Type *);
void vReturnACK(Parser_Holder_Type *);

#endif

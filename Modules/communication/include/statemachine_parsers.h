#ifndef __STATEMACHINE_PARSERS_H
#define __STATEMACHINE_PARSERS_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "serialmanager_types.h"
#include "statemachine_types.h"
#include "comlink.h"
#include "crc.h"
#include "pid.h"
#include "control.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define FIRMWARE_BASE		0x08000000
#define BOOTLOADER_BASE		0x08000000
#define SW_VERSION_OFFSET	0x184

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void vPing(Parser_Holder_Type *);
void vGetRunningMode(Parser_Holder_Type *);
void vGetBootloaderVersion(Parser_Holder_Type *);
void vGetFirmwareVersion(Parser_Holder_Type *);
void vGetRateControllerData(Parser_Holder_Type *);

#endif

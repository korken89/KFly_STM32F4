#ifndef __SERIALMANAGER_PARSERS_H
#define __SERIALMANAGER_PARSERS_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "serialmanager_types.h"
#include "comlink.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void vPrintFirmwareVersion(Parser_Holder_Type *);

#endif

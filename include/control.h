#ifndef __CONTROL_H
#define __CONTROL_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "estimation.h"
#include "filters.h"
#include "comlink.h"
#include "crc.h"
#include <math.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void ControlInit(void);
void vTaskRunControl(void *);

#endif

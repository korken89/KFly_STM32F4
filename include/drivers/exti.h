#ifndef __EXTI_H
#define __EXTI_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "led.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Global variable defines */

/* Typedefs */

/* Global function defines */
void SensorInterruptInit(void);
xSemaphoreHandle GetMPUSyncHandle(void);
xSemaphoreHandle GetHMCSyncHandle(void);
void EXTI15_10_IRQHandler(void);

#endif

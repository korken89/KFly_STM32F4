#ifndef __MAIN_H
#define __MAIN_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "usbd_cdc_core.h"
#include "usbd_cdc.h"
#include "usbd_usr.h"
#include "usbd_desc.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Driver Incudes */
#include "comlink.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "hmc5883l.h"
#include "crc.h"
#include "serialmanager.h"

/* Includes */
#include "led.h"

void main(void);
void vTaskCode(void *);
void vTaskPrintTimer(void *);

#endif

#ifndef __STATEMACHINE_GENERATORS_H
#define __STATEMACHINE_GENERATORS_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "usbd_cdc.h"
#include "serialmanager_types.h"
#include "statemachine_types.h"
#include "comlink.h"
#include "crc.h"
#include "pid.h"
#include "control.h"
#include "circularbuffer.h"

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
ErrorStatus GenerateAUXMessage(KFly_Command_Type command, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateUSBMessage(KFly_Command_Type command);
ErrorStatus GenerateHeaderOnlyCommand(KFly_Command_Type command, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGenericCommand(KFly_Command_Type command, uint8_t *data, const uint32_t data_count, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateACK(Circular_Buffer_Type *Cbuff);
ErrorStatus GeneratePing(Circular_Buffer_Type *Cbuff);

#endif

#ifndef __STATEMACHINE_GENERATORS_H
#define __STATEMACHINE_GENERATORS_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "usbd_cdc.h"
#include "version_information.h"
#include "serialmanager_types.h"
#include "serialmanager.h"
#include "statemachine_types.h"
#include "comlink.h"
#include "crc.h"
#include "pid.h"
#include "sensor_read.h"
#include "sensor_calibration.h"
#include "control.h"
#include "circularbuffer.h"
#include "estimation.h"
#include "ext_input.h"

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
ErrorStatus GenerateAUXMessage(KFly_Command_Type command, Port_Type port);
ErrorStatus GenerateUSBMessage(KFly_Command_Type command);
ErrorStatus GenerateHeaderOnlyCommand(KFly_Command_Type command, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGenericCommand(KFly_Command_Type command, uint8_t *data, const uint32_t data_count, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateACK(Circular_Buffer_Type *Cbuff);
ErrorStatus GeneratePing(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateDebugMessage(uint8_t *data, uint32_t size, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetRunningMode(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetDeviceInfo(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetRateControllerData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetAttitudeControllerData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetVelocityControllerData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetPositionControllerData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetChannelMix(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetRCCalibration(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetRCValues(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetSensorData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetRawSensorData(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetSensorCalibration(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetSensorCalibration(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetEstimationRate(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetEstimationAttitude(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetEstimationVelocity(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetEstimationPosition(Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGetEstimationAllStates(Circular_Buffer_Type *Cbuff);
uint32_t myStrlen(const uint8_t *str, const uint32_t max_length);

#endif

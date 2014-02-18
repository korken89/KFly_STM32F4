#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "comlink.h"
#include "crc.h"
#include "serialmanager_parsers.h"
#include "serialmanager_types.h"

/* Scheduler includes. */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */
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

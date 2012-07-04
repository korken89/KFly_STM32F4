#ifndef __SERIALMANAGER_H
#define __SERIALMANAGER_H

/* Standard includes */
#include "stm32f4xx.h"

/* Driver includes */
#include "comlink.h"
#include "crc.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define SYNC_BYTE						(0xa6)
#define tskSerialManagerPRIORITY		(tskIDLE_PRIORITY + 1)
#define SERIAL_BUFFER_SIZE				256
#define ACK_BIT							(0x40)

/* Typedefs */
typedef enum
{	/* This will help the parser function to identiy the Port receving data */
	PORT_USB = 0,
	PORT_AUX1,
	PORT_AUX2,
	PORT_AUX3,
	PORT_CAN
} Reveiver_Source_Type;

/* These are all the commands for the serial protocol. */
typedef enum
{
	Cmd_None = 0,
	Cmd_ACK = 1,
	Cmd_Ping,
	Cmd_DebugMessage,
	Cmd_GetRunningMode,
	Cmd_WriteFirmware,			/* Bootloader specific */
	Cmd_ReadFirmware,			/* Bootloader specific */
	Cmd_ExitBootloader,			/* Bootloader specific */
	Cmd_GetBootloaderVersion,
	Cmd_GetFirmwareVersion,
	Cmd_SaveToFlash,
	Cmd_GetRegulatorData,
	Cmd_SetRegulatorData,
	Cmd_GetChannelMix,
	Cmd_SetChannelMix,
	Cmd_StartRCCalibration,
	Cmd_StopRCCalibration,
	Cmd_CalibrateRCCenters,
	Cmd_GetRCCalibration,
	Cmd_SetRCCalibration,
	Cmd_GetRCValues,
	Cmd_GetDataDump,
	Cmd_EndOfCommands
} KFly_Command_Type;

/* Length of commands */
typedef enum {
	PingLength = 0,
	GetRunningModeLength = 0,
	WriteFirmwareLength,			/* Bootloader specific */
	ReadFirmwareLength,				/* Bootloader specific */
	ExitBootloaderLength = 0,		/* Bootloader specific */
	GetBootloaderVersionLength = 0,
	GetFirmwareVersionLength = 0,
	SaveToFlashLength = 0,
	GetRegulatorDataLength = 0,
	SetRegulatorDataLength,
	GetChannelMixLength = 0,
	SetChannelMixLength,
	StartRCCalibrationLength = 0,
	StopRCCalibrationLength = 0,
	CalibrateRCCentersLength = 0,
	GetRCCalibrationLength = 0,
	SetRCCalibrationLength,
	GetRCValuesLength,
	GetDataDumpLength = 0
} KFly_Data_Length_Type;

typedef struct _parser_holder
{
	Reveiver_Source_Type Port;
	KFly_Data_Length_Type data_length;
	uint8_t buffer[SERIAL_BUFFER_SIZE];
	uint32_t buffer_count;
	uint32_t rx_error;
	void (*current_state)(uint8_t, struct _parser_holder *);
	void (*next_state)(uint8_t, struct _parser_holder *);
	void (*parser)(struct _parser_holder *);
} Parser_Holder_Type;

/* Global variable defines */

/* Global function defines */
void vInitSerialManager(void);
void vTaskUSBSerialManager(void *);
void vWaitingForSYNC(uint8_t, Parser_Holder_Type *);
void vWaitingForSYNCorCMD(uint8_t, Parser_Holder_Type *);
void vRxCmd(uint8_t, Parser_Holder_Type *);
void vRxSize(uint8_t, Parser_Holder_Type *);
void vRxCRC8(uint8_t, Parser_Holder_Type *);
void vRxData(uint8_t, Parser_Holder_Type *);
void vRxCRC16(uint8_t, Parser_Holder_Type *);
void vReturnACK(Parser_Holder_Type *);
void vPrintFirmwareVersion(Parser_Holder_Type *);

#endif

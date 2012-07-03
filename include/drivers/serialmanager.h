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
	None = 0,
	Ping = 1,
	DebugMessage,
	GetRunningMode,
	WriteFirmware,
	ReadFirmware,
	ExitBootloader,
	GetBootloaderVersion,
	GetFirmwareVersion,
	SaveToFlash,
	GetRegulatorData,
	SetRegulatorData,
	GetChannelMix,
	SetChannelMix,
	StartRCCalibration,
	StopRCCalibration,
	CalibrateRCCenters,
	GetRCCalibration,
	SetRCCalibration,
	GetRCValues,
	EndOfCommands
} KFly_Command_Type;

/* Length of commands */
typedef enum {
	PingLength = 0,
	GetRunningModeLength = 0,
	WriteFirmwareLength,
	ReadFirmwareLength,
	ExitBootloaderLength = 0,
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
	GetRCValuesLength
} KFly_Data_Length_Type;

typedef struct _parser_holder
{
	Reveiver_Source_Type Port;
	KFly_Data_Length_Type data_length;
	uint32_t bytes_received;
	uint8_t buffer[SERIAL_BUFFER_SIZE];
	uint32_t rx_error;
	void (*current_state)(uint8_t, struct _parser_holder *);
	void (*next_state)(uint8_t, struct _parser_holder *);
	void (*parser)(uint8_t, struct _parser_holder *);
} Parser_Holder_Type;

/* Global variable defines */

/* Global function defines */
void vInitSerialManager(void);
void vTaskUSBSerialManager(void *);
void vWaitingForSYNC(uint8_t, Parser_Holder_Type *);
void vWaitingForCMD(uint8_t, Parser_Holder_Type *);
void vWaitingForSYNCorCMD(uint8_t, Parser_Holder_Type *);
void vRxCmd(uint8_t, Parser_Holder_Type *);

#endif

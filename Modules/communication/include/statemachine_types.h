#ifndef __STATEMACHINE_TYPES_H
#define __STATEMACHINE_TYPES_H

#include "serialmanager_types.h"
#include "circularbuffer.h"

#define SYNC_BYTE				(0xa6)
#define ACK_BIT					(0x80)
#define SERIAL_BUFFER_SIZE		(256)

/* These are all the commands for the serial protocol. */
typedef enum
{
	Cmd_None = 0,
	Cmd_ACK = 1,
	Cmd_Ping = 2,
	Cmd_DebugMessage = 3,
	Cmd_GetRunningMode = 4,

	Cmd_PrepareWriteFirmware = 10,   	/* Bootloader specific, shall always require ACK */
	Cmd_WriteFirmwarePackage = 11,		/* Bootloader specific, shall always require ACK */
	Cmd_WriteLastFirmwarePackage = 12,	/* Bootloader specific, shall always require ACK */
	Cmd_ReadFirmwarePackage = 13,		/* Bootloader specific, shall always require ACK */
	Cmd_ReadLastFirmwarePackage = 14,   /* Bootloader specific, shall always require ACK */
	Cmd_NextPackage = 15,   			/* Bootloader specific, shall always require ACK */
	Cmd_ExitBootloader = 16,   			/* Bootloader specific, shall always require ACK */
	Cmd_GetBootloaderVersion = 17,
	Cmd_GetFirmwareVersion = 18,
	Cmd_SaveToFlash = 19,

	Cmd_GetRateControllerData = 30,
	Cmd_SetRateControllerData = 31,
	Cmd_GetAttitudeControllerData = 32,
	Cmd_SetAttitudeControllerData = 33,
	Cmd_GetVelocityControllerData = 34,
	Cmd_SetVelocityControllerData = 35,
	Cmd_GetPositionControllerData = 36,
	Cmd_SetPositionControllerData = 37,
	Cmd_RESERVED = 38,
	Cmd_GetChannelMix = 39,
	Cmd_SetChannelMix = 40,
	Cmd_GetRCCalibration = 41,
	Cmd_SetRCCalibration = 42,
	Cmd_GetRCValues = 43,
	Cmd_GetSensorData = 44
} KFly_Command_Type;

/* The structure to keep track of transfers through the state machine */
typedef struct _parser_holder
{
	Receiver_Source_Type Port; 									/* Which port the data came from */
	uint8_t data_length;										/* The length of the data */
	uint8_t *buffer;											/* Pointer to the buffer storing the data */
	uint16_t buffer_count;										/* The current location in the buffer */
	uint32_t rx_error;											/* The number of receive errors */
	void (*current_state)(uint8_t, struct _parser_holder *);	/* Current state in the state machine */
	void (*next_state)(uint8_t, struct _parser_holder *);		/* Next state in the state machine */
	void (*parser)(struct _parser_holder *);					/* Parser to parse the data after a successful transfer */
	Bool AckRequested;											/* If an ACK was requested */
} Parser_Holder_Type;

/* Function pointer definition for the Message Parser lookup table */
typedef void (*Parser_Type)(struct _parser_holder *);

/* Function pointer definition for the Message Generator lookup table */
typedef ErrorStatus (*Generator_Type)(Circular_Buffer_Type *);


#endif

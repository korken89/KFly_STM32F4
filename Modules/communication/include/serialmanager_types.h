#ifndef __SERIALMANAGER_TYPES_H
#define __SERIALMANAGER_TYPES_H

#define SYNC_BYTE						(0xa6)
#define ACK_BIT							(0x40)
#define SERIAL_BUFFER_SIZE				256

typedef enum
{	/* This will help the parser function to identity the Port receiving data */
	PORT_USB = 0,
	PORT_AUX1,
	PORT_AUX2,
	PORT_AUX3,
	PORT_AUX4		/* CAN port */
} Reveiver_Source_Type;

/* These are all the commands for the serial protocol. */
typedef enum
{
	Cmd_None = 0,
	Cmd_ACK = 1,
	Cmd_Ping,
	Cmd_DebugMessage,
	Cmd_GetRunningMode,
	Cmd_PrepareWriteFirmware,		/* Bootloader specific, shall always require ACK */
	Cmd_WriteFirmwarePackage,		/* Bootloader specific, shall always require ACK */
	Cmd_WriteLastFirmwarePackage,	/* Bootloader specific, shall always require ACK */
	Cmd_ReadFirmwarePackage,		/* Bootloader specific, shall always require ACK */
	Cmd_ReadLastFirmwarePackage,	/* Bootloader specific, shall always require ACK */
	Cmd_NextPackage,				/* Bootloader specific, shall always require ACK */
	Cmd_ExitBootloader,				/* Bootloader specific, shall always require ACK */
	Cmd_GetBootloaderVersion,
	Cmd_GetFirmwareVersion,
	Cmd_SaveToFlash,
	Cmd_GetControllerData,
	Cmd_SetControllerData,
	Cmd_GetChannelMix,
	Cmd_SetChannelMix,
	Cmd_StartRCCalibration,
	Cmd_StopRCCalibration,
	Cmd_CalibrateRCCenters,
	Cmd_GetRCCalibration,
	Cmd_SetRCCalibration,
	Cmd_GetRCValues,
	Cmd_GetSensorData,
	EndOfCommands
} KFly_Command_Type;

/* Length of data packets */
typedef enum {
	Length_ACK = 0,
	Length_Ping = 0,
	Length_DebugMessage = 0xFF,
	Length_GetRunningMode = 0,
	Length_PrepareWriteFirmware,			/* Bootloader specific, tells bootloader to erase and prepare the flash for write */
	Length_WriteFirmwarePackage = 66,		/* Bootloader specific, package of 2 bytes Package no, 64 bytes data */
	Length_WriteLastFirmwarePackage = 0xFF,	/* Bootloader specific, package of 2 bytes Package no, unknown number of bytes */
	Length_ReadFirmwarePackage = 66,		/* Bootloader specific, package of 2 bytes Package no, 64 bytes data */
	Length_ReadLastFirmwarePackage = 0xFF,	/* Bootloader specific, package of 2 bytes Package no, unknown number of bytes */
	Length_NextPackage = 0,					/* Bootloader specific, tells the sender that Flash write is complete and is ready for next package */
	Length_ExitBootloader = 0,				/* Bootloader specific, exits bootloader */
	Length_GetBootloaderVersion = 0,
	Length_GetFirmwareVersion = 0,
	Length_SaveToFlash = 0,
	Length_GetControllerData = 0,
	Length_SetControllerData,
	Length_GetChannelMix = 0,
	Length_SetChannelMix,
	Length_StartRCCalibration = 0,
	Length_StopRCCalibration = 0,
	Length_CalibrateRCCenters = 0,
	Length_GetRCCalibration = 0,
	Length_SetRCCalibration,
	Length_GetRCValues,
	Length_GetSensorData
} KFly_Data_Length_Type;

/* The structure to keep track of transfers through the state machine */
typedef struct _parser_holder
{
	Reveiver_Source_Type Port; 									/* Which port the data came from */
	KFly_Data_Length_Type data_length;							/* The length of the data */
	uint8_t buffer[SERIAL_BUFFER_SIZE];							/* The buffer storing the data */
	uint32_t buffer_count;										/* The current location in the buffer */
	uint32_t rx_error;											/* The number of receive errors */
	void (*current_state)(uint8_t, struct _parser_holder *);	/* Current state in the state machine */
	void (*next_state)(uint8_t, struct _parser_holder *);		/* Next state in the state machine */
	void (*parser)(struct _parser_holder *);					/* Parser to parse the data after a successful transfer */
} Parser_Holder_Type;

typedef union
{
	uint8_t data[4];
	float value;
} f2bArray;

typedef union
{
	uint8_t data[2];
	int16_t value;
} sint2bArray;

typedef union
{
	uint8_t data[4];
	int32_t value;
} int2bArray;

#endif

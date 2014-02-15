#ifndef __SERIALMANAGER_TYPES_H
#define __SERIALMANAGER_TYPES_H

#define SYNC_BYTE				(0xa6)
#define ACK_BIT					(0x40)
#define SERIAL_BUFFER_SIZE		(256)

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
	Cmd_GetChannelMix = 38,
	Cmd_SetChannelMix = 39,
	Cmd_GetRCCalibration = 40,
	Cmd_SetRCCalibration = 41,
	Cmd_GetRCValues = 42,
	Cmd_GetSensorData = 43
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
	uint8_t *buffer;											/* Pointer to the buffer storing the data */
	uint16_t buffer_count;										/* The current location in the buffer */
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

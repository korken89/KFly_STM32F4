/* *
 *
 * All the parsers for the different packages.
 * To expand the functionality of the serial com is just to add
 * functions here and add them to the serial com switch-statement.
 *
 * */

#include "stdint.h"
#include "statemachine_parsers.h"

/**
 * Lookup table for all the serial parsers.
 */
static const Parser_Type parser_lookup[128] = {
	NULL,						/* 0: 	Cmd_None 						*/
	NULL,						/* 1: 	Cmd_ACK 						*/
	ParsePing,					/* 2: 	Cmd_Ping 						*/
	NULL,						/* 3:	Cmd_DebugMessage				*/
	ParseGetRunningMode,		/* 4:	Cmd_GetRunningMode 				*/
	NULL,						/* 5 */
	NULL,						/* 6 */
	NULL,						/* 7 */
	NULL,						/* 8 */
	NULL,						/* 9 */
	NULL,						/* 10:	Cmd_PrepareWriteFirmware 		*/
	NULL,						/* 11: 	Cmd_WriteFirmwarePackage 		*/
	NULL,						/* 12:	Cmd_WriteLastFirmwarePackage	*/
	NULL,						/* 13:	Cmd_ReadFirmwarePackage			*/
	NULL,						/* 14:	Cmd_ReadLastFirmwarePackage		*/
	NULL,						/* 15:	Cmd_NextPackage					*/
	NULL,						/* 16:	Cmd_ExitBootloader				*/
	ParseGetBootloaderVersion,	/* 17:	Cmd_GetBootloaderVersion		*/
	ParseGetFirmwareVersion,	/* 18:	Cmd_GetFirmwareVersion			*/
	NULL,						/* 19:	Cmd_SaveToFlash					*/
	NULL,						/* 20 */
	NULL,						/* 21 */
	NULL,						/* 22 */
	NULL,						/* 23 */
	NULL,						/* 24 */
	NULL,						/* 25 */
	NULL,						/* 26 */
	NULL,						/* 27 */
	NULL,						/* 28 */
	NULL,						/* 29 */
	ParseGetRateControllerData,	/* 30:	Cmd_GetRateControllerData		*/
	NULL,						/* 31:	Cmd_SetRateControllerData		*/
	NULL,						/* 32:	Cmd_GetAttitudeControllerData	*/
	NULL,						/* 33:	Cmd_SetAttitudeControllerData	*/
	NULL,						/* 34:	Cmd_GetVelocityControllerData	*/
	NULL,						/* 35: 	Cmd_SetVelocityControllerData 	*/
	NULL,						/* 36:	Cmd_GetPositionControllerData	*/
	NULL,						/* 37:	Cmd_SetPositionControllerData	*/
	NULL,						/* 38:	RESERVED						*/
	NULL,						/* 39:	Cmd_GetChannelMix				*/
	NULL,						/* 40:	Cmd_SetChannelMix				*/
	NULL,						/* 41:	Cmd_GetRCCalibration			*/
	NULL,						/* 42:	Cmd_SetRCCalibration			*/
	NULL,						/* 43:	Cmd_GetRCValues					*/
	NULL,						/* 44:	Cmd_GetSensorData				*/
	NULL,						/* 45 */
	NULL,						/* 46 */
	NULL,						/* 47 */
	NULL,						/* 48 */
	NULL,						/* 49 */
	NULL,						/* 50 */
	NULL,						/* 51 */
	NULL,						/* 52 */
	NULL,						/* 53 */
	NULL,						/* 54 */
	NULL,						/* 55 */
	NULL,						/* 56 */
	NULL,						/* 57 */
	NULL,						/* 58 */
	NULL,						/* 59 */
	NULL,						/* 60 */
	NULL,						/* 61 */
	NULL,						/* 62 */
	NULL,						/* 63 */
	NULL,						/* 64 */
	NULL,						/* 65 */
	NULL,						/* 66 */
	NULL,						/* 67 */
	NULL,						/* 68 */
	NULL,						/* 69 */
	NULL,						/* 70 */
	NULL,						/* 71 */
	NULL,						/* 72 */
	NULL,						/* 73 */
	NULL,						/* 74 */
	NULL,						/* 75 */
	NULL,						/* 76 */
	NULL,						/* 77 */
	NULL,						/* 78 */
	NULL,						/* 79 */
	NULL,						/* 80 */
	NULL,						/* 81 */
	NULL,						/* 82 */
	NULL,						/* 83 */
	NULL,						/* 84 */
	NULL,						/* 85 */
	NULL,						/* 86 */
	NULL,						/* 87 */
	NULL,						/* 88 */
	NULL,						/* 89 */
	NULL,						/* 90 */
	NULL,						/* 91 */
	NULL,						/* 92 */
	NULL,						/* 93 */
	NULL,						/* 94 */
	NULL,						/* 95 */
	NULL,						/* 96 */
	NULL,						/* 97 */
	NULL,						/* 98 */
	NULL,						/* 99 */
	NULL,						/* 100 */
	NULL,						/* 101 */
	NULL,						/* 102 */
	NULL,						/* 103 */
	NULL,						/* 104 */
	NULL,						/* 105 */
	NULL,						/* 106 */
	NULL,						/* 107 */
	NULL,						/* 108 */
	NULL,						/* 109 */
	NULL,						/* 110 */
	NULL,						/* 111 */
	NULL,						/* 112 */
	NULL,						/* 113 */
	NULL,						/* 114 */
	NULL,						/* 115 */
	NULL,						/* 116 */
	NULL,						/* 117 */
	NULL,						/* 118 */
	NULL,						/* 119 */
	NULL,						/* 120 */
	NULL,						/* 121 */
	NULL,						/* 122 */
	NULL,						/* 123 */
	NULL,						/* 124 */
	NULL,						/* 125 */
	NULL,						/* 126 */
	NULL						/* 127 */
};


Parser_Type GetParser(KFly_Command_Type command)
{
	return parser_lookup[command];
}


/* *
 * Sends a ping.
 * */
void ParsePing(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_Ping);
}

/* *
 * Get the current running mode. P for program, B for bootloader.
 * */
void ParseGetRunningMode(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRunningMode);
	else if (pHolder->Port == PORT_AUX1)
		GenerateAUXMessage(Cmd_GetRunningMode, NULL);
}

/* *
 * Gets the bootloader version and sends it.
 * */
void ParseGetBootloaderVersion(Parser_Holder_Type *pHolder)
{
	

	
	uint8_t *msg;
	msg = (uint8_t *)(BOOTLOADER_BASE + SW_VERSION_OFFSET); /* Text is at known location */
	uint8_t str[128];
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetBootloaderVersion;

	while (txt && (i < 60)) /* In case there is something wrong with the string  */
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 4;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i);
}

/* *
 * Gets the firmware version and sends it.
 * */
void ParseGetFirmwareVersion(Parser_Holder_Type *pHolder)
{
	uint8_t *msg;
	msg = (uint8_t *)(FIRMWARE_BASE + SW_VERSION_OFFSET); /* Text is at known location */
	uint8_t str[128];
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetFirmwareVersion;

	while (txt && (i < 60)) /* In case there is something wrong with the string  */
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 4;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i);
}

void ParseGetRateControllerData(Parser_Holder_Type *pHolder)
{
	uint8_t str[100];
	PI_Data_Type *PI_settings;
	uint8_t *CL_settings;
	uint8_t *data;
	uint32_t size = 4;
	int i, j;
	uint16_t crc16;

	/* CasT the control data to an array of PI_Data_Type */
	PI_settings = (PI_Data_Type *)ptrGetControlData();

	/* Cast the settings into bytes for read out */
	CL_settings = (uint8_t *)ptrGetControlLimits();

	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetRateControllerData;
	str[2] = 48;
	str[3] = CRC8(str, 3);

	for (i = 0; i < 3; i++) /* Get only the rate coefficients */
	{
		data = (uint8_t *)&PI_settings[i];

		for (j = 0; j < 12; j++)
			str[size++] = data[j];
	}

	for (i = 0; i < 12; i++) /* Get only the rate constraints */
	{
		str[size++] = CL_settings[i];
	}

	crc16 = CRC16(str, size - 1);

	str[size++] = (uint8_t)(crc16 >> 8);
	str[size++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, size);
}

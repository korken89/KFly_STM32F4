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
	NULL,								/* 0: 	Cmd_None 						*/
	NULL,								/* 1: 	Cmd_ACK 						*/
	ParsePing,							/* 2: 	Cmd_Ping 						*/
	NULL,								/* 3:	Cmd_DebugMessage				*/
	ParseGetRunningMode,				/* 4:	Cmd_GetRunningMode 				*/
	NULL,								/* 5 */
	NULL,								/* 6 */
	NULL,								/* 7 */
	NULL,								/* 8 */
	NULL,								/* 9 */
	NULL,								/* 10:	Cmd_PrepareWriteFirmware 		*/
	NULL,								/* 11: 	Cmd_WriteFirmwarePackage 		*/
	NULL,								/* 12:	Cmd_WriteLastFirmwarePackage	*/
	NULL,								/* 13:	Cmd_ReadFirmwarePackage			*/
	NULL,								/* 14:	Cmd_ReadLastFirmwarePackage		*/
	NULL,								/* 15:	Cmd_NextPackage					*/
	NULL,								/* 16:	Cmd_ExitBootloader				*/
	ParseGetBootloaderVersion,			/* 17:	Cmd_GetBootloaderVersion		*/
	ParseGetFirmwareVersion,			/* 18:	Cmd_GetFirmwareVersion			*/
	ParseSaveToFlash,					/* 19:	Cmd_SaveToFlash					*/
	NULL,								/* 20 */
	NULL,								/* 21 */
	NULL,								/* 22 */
	NULL,								/* 23 */
	NULL,								/* 24 */
	NULL,								/* 25 */
	NULL,								/* 26 */
	NULL,								/* 27 */
	NULL,								/* 28 */
	NULL,								/* 29 */
	ParseGetRateControllerData,			/* 30:	Cmd_GetRateControllerData		*/
	ParseSetRateControllerData,			/* 31:	Cmd_SetRateControllerData		*/
	ParseGetAttitudeControllerData,		/* 32:	Cmd_GetAttitudeControllerData	*/
	ParseSetAttitudeControllerData,		/* 33:	Cmd_SetAttitudeControllerData	*/
	ParseGetVelocityControllerData,		/* 34:	Cmd_GetVelocityControllerData	*/
	ParseSetVelocityControllerData,		/* 35: 	Cmd_SetVelocityControllerData 	*/
	ParseGetPositionControllerData,		/* 36:	Cmd_GetPositionControllerData	*/
	ParseSetPositionControllerData,		/* 37:	Cmd_SetPositionControllerData	*/
	NULL,								/* 38:	RESERVED						*/
	ParseGetChannelMix,					/* 39:	Cmd_GetChannelMix				*/
	ParseSetChannelMix,					/* 40:	Cmd_SetChannelMix				*/
	ParseGetRCCalibration,				/* 41:	Cmd_GetRCCalibration			*/
	ParseSetRCCalibration,				/* 42:	Cmd_SetRCCalibration			*/
	ParseGetRCValues,					/* 43:	Cmd_GetRCValues					*/
	ParseGetSensorData,					/* 44:	Cmd_GetSensorData				*/
	NULL,								/* 45 */
	NULL,								/* 46 */
	NULL,								/* 47 */
	NULL,								/* 48 */
	NULL,								/* 49 */
	NULL,								/* 50 */
	NULL,								/* 51 */
	NULL,								/* 52 */
	NULL,								/* 53 */
	NULL,								/* 54 */
	NULL,								/* 55 */
	NULL,								/* 56 */
	NULL,								/* 57 */
	NULL,								/* 58 */
	NULL,								/* 59 */
	NULL,								/* 60 */
	NULL,								/* 61 */
	NULL,								/* 62 */
	NULL,								/* 63 */
	NULL,								/* 64 */
	NULL,								/* 65 */
	NULL,								/* 66 */
	NULL,								/* 67 */
	NULL,								/* 68 */
	NULL,								/* 69 */
	NULL,								/* 70 */
	NULL,								/* 71 */
	NULL,								/* 72 */
	NULL,								/* 73 */
	NULL,								/* 74 */
	NULL,								/* 75 */
	NULL,								/* 76 */
	NULL,								/* 77 */
	NULL,								/* 78 */
	NULL,								/* 79 */
	NULL,								/* 80 */
	NULL,								/* 81 */
	NULL,								/* 82 */
	NULL,								/* 83 */
	NULL,								/* 84 */
	NULL,								/* 85 */
	NULL,								/* 86 */
	NULL,								/* 87 */
	NULL,								/* 88 */
	NULL,								/* 89 */
	NULL,								/* 90 */
	NULL,								/* 91 */
	NULL,								/* 92 */
	NULL,								/* 93 */
	NULL,								/* 94 */
	NULL,								/* 95 */
	NULL,								/* 96 */
	NULL,								/* 97 */
	NULL,								/* 98 */
	NULL,								/* 99 */
	NULL,								/* 100 */
	NULL,								/* 101 */
	NULL,								/* 102 */
	NULL,								/* 103 */
	NULL,								/* 104 */
	NULL,								/* 105 */
	NULL,								/* 106 */
	NULL,								/* 107 */
	NULL,								/* 108 */
	NULL,								/* 109 */
	NULL,								/* 110 */
	NULL,								/* 111 */
	NULL,								/* 112 */
	NULL,								/* 113 */
	NULL,								/* 114 */
	NULL,								/* 115 */
	NULL,								/* 116 */
	NULL,								/* 117 */
	NULL,								/* 118 */
	NULL,								/* 119 */
	NULL,								/* 120 */
	NULL,								/* 121 */
	NULL,								/* 122 */
	NULL,								/* 123 */
	NULL,								/* 124 */
	NULL,								/* 125 */
	NULL,								/* 126 */
	NULL								/* 127 */
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
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetBootloaderVersion);
	else if (pHolder->Port == PORT_AUX1)
		GenerateAUXMessage(Cmd_GetBootloaderVersion, NULL);
}

/* *
 * Gets the firmware version and sends it.
 * */
void ParseGetFirmwareVersion(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetFirmwareVersion);
	else if (pHolder->Port == PORT_AUX1)
		GenerateAUXMessage(Cmd_GetFirmwareVersion, NULL);
}

void ParseSaveToFlash(Parser_Holder_Type *pHolder)
{
	
}

void ParseGetRateControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRateControllerData);
	else if (pHolder->Port == PORT_AUX1)
		GenerateAUXMessage(Cmd_GetRateControllerData, NULL);
}

void ParseSetRateControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseGetAttitudeControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseSetAttitudeControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseGetVelocityControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseSetVelocityControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseGetPositionControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseSetPositionControllerData(Parser_Holder_Type *pHolder)
{

}


void ParseGetChannelMix(Parser_Holder_Type *pHolder)
{

}


void ParseSetChannelMix(Parser_Holder_Type *pHolder)
{

}


void ParseGetRCCalibration(Parser_Holder_Type *pHolder)
{

}


void ParseSetRCCalibration(Parser_Holder_Type *pHolder)
{

}


void ParseGetRCValues(Parser_Holder_Type *pHolder)
{

}


void ParseGetSensorData(Parser_Holder_Type *pHolder)
{

}



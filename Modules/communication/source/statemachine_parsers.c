/* *
 *
 * All the parsers for the different packages.
 * To expand the functionality of the serial com is just to add
 * functions here and add them to the serial com switch-statement.
 *
 * */

#include "stdint.h"
#include "statemachine_parsers.h"

/* Private functions */
void ParseGenericSetControllerData(const uint32_t pi_offset, const uint32_t limit_offset, const uint32_t limit_count, uint8_t *data);

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
	ParseGetRawSensorData,				/* 45:	Cmd_GetRarSensorData 			*/
	ParseGetSensorCalibration,			/* 46:	Cmd_GetSensorCalibration 		*/
	ParseSetSensorCalibration,			/* 47:	Cmd_SetSensorCalibration 		*/
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

/**
 * @brief 			Return the parser associated to the command.
 * @details 
 * 
 * @param command 	Command to get the parser for.
 * 
 * @return 			Pointer to the associated parser.
 */
Parser_Type GetParser(KFly_Command_Type command)
{
	return parser_lookup[command];
}

/**
 * @brief A generic function to save controller data and control limits.
 * @details
 * 
 * @param pi_offset 	The offset to the correct set of PI controllers.
 * @param limit_offset 	The offset to the correct part of the limits structure.
 * @param limit_count 	Number of bytes to write to the limits structure.
 * @param data 			Input data so save.
 */
void ParseGenericSetControllerData(const uint32_t pi_offset, const uint32_t limit_offset, const uint32_t limit_count, uint8_t *data)
{
	PI_Data_Type *PI_settings;
	uint8_t *save_location;
	uint32_t i, j;

	/* Cast the control data to an array of PI_Data_Type to access each PI controller */
	PI_settings = (PI_Data_Type *)ptrGetControlData();

	/* Write only the PI coefficients */
	for (i = 0; i < 3; i++) 
	{
		save_location = (uint8_t *)&PI_settings[pi_offset + i];

		for (j = 0; j < 12; j++)
			save_location[j] = data[(i*12) + j];
	}

	/* Cast the settings into bytes for saving */
	save_location = (uint8_t *)ptrGetControlLimits();

	/* Write only the controller constraints */
	for (i = 0; i < limit_count; i++) 
		save_location[limit_offset + i] = data[(3*3*4) + i];
}

/**
 * @brief 			Parses a Ping command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParsePing(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_Ping);
	else
		GenerateAUXMessage(Cmd_Ping, pHolder->Port);
}

/**
 * @brief 			Parses a GetRunningMode command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetRunningMode(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRunningMode);
	else
		GenerateAUXMessage(Cmd_GetRunningMode, pHolder->Port);
}

/**
 * @brief 			Parses a GetBootloaderVersion command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetBootloaderVersion(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetBootloaderVersion);
	else
		GenerateAUXMessage(Cmd_GetBootloaderVersion, pHolder->Port);
}

/**
 * @brief 			Parses a GetFirmwareVersion command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetFirmwareVersion(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetFirmwareVersion);
	else
		GenerateAUXMessage(Cmd_GetFirmwareVersion, pHolder->Port);
}

/**
 * @brief 			Parses a SaveToFlash command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSaveToFlash(Parser_Holder_Type *pHolder)
{

}

/**
 * @brief 			Parses a GetRateControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetRateControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRateControllerData);
	else
		GenerateAUXMessage(Cmd_GetRateControllerData, pHolder->Port);
}

/**
 * @brief 			Parses a SetRateControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetRateControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->buffer_count == (12 + RATE_LIMIT_COUNT))
		ParseGenericSetControllerData(RATE_PI_OFFSET, RATE_LIMIT_OFFSET, RATE_LIMIT_COUNT, pHolder->buffer);
}

/**
 * @brief 			Parses a GetAttitudeControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetAttitudeControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetAttitudeControllerData);
	else
		GenerateAUXMessage(Cmd_GetAttitudeControllerData, pHolder->Port);
}

/**
 * @brief 			Parses a SetAttitudeControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetAttitudeControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->buffer_count == (12 + ATTITUDE_LIMIT_COUNT))
		ParseGenericSetControllerData(ATTITUDE_PI_OFFSET, ATTITUDE_LIMIT_OFFSET, ATTITUDE_LIMIT_COUNT, pHolder->buffer);
}

/**
 * @brief 			Parses a GetVelocityControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetVelocityControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetVelocityControllerData);
	else
		GenerateAUXMessage(Cmd_GetVelocityControllerData, pHolder->Port);
}

/**
 * @brief 			Parses a SetVelocityControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetVelocityControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->buffer_count == (12 + VELOCITY_LIMIT_COUNT))
		ParseGenericSetControllerData(VELOCITY_PI_OFFSET, VELOCITY_LIMIT_OFFSET, VELOCITY_LIMIT_COUNT, pHolder->buffer);	
}

/**
 * @brief 			Parses a GetPositionControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetPositionControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetPositionControllerData);
	else
		GenerateAUXMessage(Cmd_GetPositionControllerData, pHolder->Port);
}

/**
 * @brief 			Parses a SetPositionControllerData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetPositionControllerData(Parser_Holder_Type *pHolder)
{
	if (pHolder->buffer_count == (12 + POSITION_LIMIT_COUNT))
		ParseGenericSetControllerData(POSITION_PI_OFFSET, POSITION_LIMIT_OFFSET, POSITION_LIMIT_COUNT, pHolder->buffer);
}

/**
 * @brief 			Parses a GetChannelMix command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetChannelMix(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetChannelMix);
	else
		GenerateAUXMessage(Cmd_GetChannelMix, pHolder->Port);
}

/**
 * @brief 			Parses a SetChannelMix command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetChannelMix(Parser_Holder_Type *pHolder)
{
	uint32_t i;
	uint8_t *save_location;

	if (pHolder->buffer_count == (4*8*4))
	{
		save_location = (uint8_t *)ptrGetOutputMixer();

		for (i = 0; i < (4*8*4); i++)
			save_location[i] = pHolder->buffer[i];
	}
}

/**
 * @brief 			Parses a GetRCCalibration command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetRCCalibration(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRCCalibration);
	else
		GenerateAUXMessage(Cmd_GetRCCalibration, pHolder->Port);
}

/**
 * @brief 			Parses a SetRCCalibration command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetRCCalibration(Parser_Holder_Type *pHolder)
{

}

/**
 * @brief 			Parses a GetRCValues command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetRCValues(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRCValues);
	else
		GenerateAUXMessage(Cmd_GetRCValues, pHolder->Port);
}

/**
 * @brief 			Parses a GetSensorData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetSensorData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetSensorData);
	else
		GenerateAUXMessage(Cmd_GetSensorData, pHolder->Port);
}

/**
 * @brief 			Parses a GetRawSensorData command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetRawSensorData(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetRawSensorData);
	else
		GenerateAUXMessage(Cmd_GetRawSensorData, pHolder->Port);
}

/**
 * @brief 			Parses a GetSensorCalibration command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseGetSensorCalibration(Parser_Holder_Type *pHolder)
{
	if (pHolder->Port == PORT_USB)
		GenerateUSBMessage(Cmd_GetSensorCalibration);
	else
		GenerateAUXMessage(Cmd_GetSensorCalibration, pHolder->Port);
}

/**
 * @brief 			Parses a SetSensorCalibration command.
 * @details
 * 
 * @param pHolder 	Message holder containing information about the transmission. 
 */
void ParseSetSensorCalibration(Parser_Holder_Type *pHolder)
{
	uint32_t i;
	uint8_t *save_location;

	if (pHolder->buffer_count == (4*3*4))
	{
		save_location = (uint8_t *)ptrGetSensorCalibration();

		for (i = 0; i < (4*3*4); i++)
			save_location[i] = pHolder->buffer[i];
	}
}
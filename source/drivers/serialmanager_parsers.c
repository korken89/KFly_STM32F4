/* *
 *
 * All the parsers for the different packages.
 * To expand the functionality of the serial com is just to add
 * functions here and add them to the serial com switch-statement.
 *
 * */

#include "serialmanager_parsers.h"

/* *
 * Gets the bootloader version and sends it.
 * */
void vGetBootloaderVersion(Parser_Holder_Type *pHolder)
{
	uint8_t *msg;
	msg = (uint8_t *)(BOOTLOADER_BASE + SW_VERSION_OFFSET);
	uint8_t str[128] = {0};
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetFirmwareVersion;

	while (txt)
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 5;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i - 1);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i - 1);
}

/* *
 * Gets the firmware version and sends it.
 * */
void vGetFirmwareVersion(Parser_Holder_Type *pHolder)
{
	uint8_t *msg;
	msg = (uint8_t *)(FIRMWARE_BASE + SW_VERSION_OFFSET);
	uint8_t str[128] = {0};
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetFirmwareVersion;

	while (txt)
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 5;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i - 1);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i - 1);
}

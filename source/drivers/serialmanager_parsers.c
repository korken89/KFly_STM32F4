#include "serialmanager_parsers.h"


void vPrintFirmwareVersion(Parser_Holder_Type *pHolder)
{
	extern int _eisr_vector;
	uint8_t str[128] = {0};
	int i = 0;
	uint8_t txt;
	/* TODO: Add message holder */

	do
	{
		txt = *((uint8_t *)&_eisr_vector + i);
		str[i++] = txt;
	} while (txt);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i-1);
}

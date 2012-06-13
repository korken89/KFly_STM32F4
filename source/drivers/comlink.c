#include "comlink.h"

/* Gobal variable defines */
volatile xQueueHandle xUSBQueue;

/* Private function defines */


/* *
 * vUSBQueueInit initializes the queue for USB recieved data
 * */
void vUSBQueueInit(void)
{
	xUSBQueue = xQueueCreate(USBQueueSize, sizeof(uint8_t));
}

/* *
 *
 * Hardware Abstraction Layer for all comunication
 *
 * TODO:
 * Add support for Serial Communication AUX1, AUX2 and AUX3
 *
 * */


#include "comlink.h"

/* Gobal variable defines */
volatile xQueueHandle xUSBQueueHandle = NULL;
volatile Bool bUSBAvalible = FALSE;

/* Private function defines */

/* Private external functions */
extern uint16_t cdc_DataTx(uint8_t *, uint32_t);

/* *
 * vUSBQueueInit initializes the queue for USB recieved data
 * */
void vUSBQueueInit(void)
{
	xUSBQueueHandle = xQueueCreate(USBQueueSize, sizeof(uint8_t));
}

/* *
 *
 * Function for sending data over USB.
 * Theoretical max speed is 1.6M baud however a much slower rate is
 * recommened because the in and out endpoints uses the same buffer.
 * 115200 buad is normal operating conditions. (Higher can be used)
 *
 * Please observe that this function is NOT ISR-safe for the RTOS.
 * If called from ISR it can break pakages sent via the RTOS.
 *
 * */
ErrorStatus xUSBSendData(uint8_t *data, uint32_t size)
{
	/*  if USB is not avalible, signal with error. */
	if (bUSBAvalible == FALSE)
		return ERROR;

	else
	{
		vTaskSuspendAll();
		{
			cdc_DataTx(data, size);
		}
		xTaskResumeAll();

		return SUCCESS;
	}
}

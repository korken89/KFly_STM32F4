/* *
 *
 * Hardware Abstraction Layer for all communications
 *
 * TODO:
 * Add support for Serial Communication AUX1, AUX2, AUX3 and CAN1
 *
 * */


#include "comlink.h"

/* Global variable defines */
volatile USB_QUEUE_Type xUSBQueue = {NULL, FALSE};

/* Private function defines */

/* Private external functions */
extern uint16_t cdc_DataTx(uint8_t *, uint32_t);

/* *
 * vUSBQueueInit initializes the queue for USB received data
 * */
void vUSBQueueInit(void)
{
	xUSBQueue.xUSBQueueHandle = xQueueCreate(xUSBQueueSize, sizeof(uint8_t));
}

/* *
 *
 * Function for sending data over USB.
 * Theoretical max speed is 1.6M baud however a much slower rate is
 * recommended because the in and out end points uses the same buffer.
 * 115200 Baud is normal operating conditions. (Higher can be used)
 *
 * It's also important to send an entire message at a time, else
 * someone might send a message in between, corrupting the data stream.
 *
 * Please observe that this function is NOT ISR-safe for the RTOS.
 * If called from ISR it can break packages sent via the RTOS.
 *
 * */
ErrorStatus xUSBSendData(uint8_t *data, uint32_t size)
{
	/*  if USB is not available, signal with error. */
	if (xUSBQueue.bUSBAvalible == FALSE)
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

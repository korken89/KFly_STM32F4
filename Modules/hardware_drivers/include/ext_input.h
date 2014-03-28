#ifndef __EXT_INPUT_H
#define __EXT_INPUT_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes. */

/* KFly includes */

/* Driver Includes */

/* Includes */

/* Defines */
#define MAX_NUMBER_OF_INPUTS		12

/* Typedefs */
typedef struct {
	Bool active_connection;
	int16_t value[MAX_NUMBER_OF_INPUTS];
	float rssi;
} Raw_External_Input_Type;

typedef enum {
	INPUT_CH1 = 0,
	INPUT_CH2 = 1,
	INPUT_CH3 = 2,
	INPUT_CH4 = 3,
	INPUT_CH5 = 4,
	INPUT_CH6 = 5,
	INPUT_CPPM = 6,
	INPUT_RSSI = 7
} Input_Capture_Channel;

/* Global functions */
void Input_CPPM_RSSI_Config(void);
void Input_PWM_Config(void);

#endif

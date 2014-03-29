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

#define CAPTURE_TIMER_RATE 			1000000
#define MAX_NUMBER_OF_INPUTS		12
#define CPPM_SYNC_LIMIT_MIN			3000	/* 3 ms */
#define CPPM_SYNC_LIMIT_MAX			30000	/* 30 ms */

/* Typedefs */
typedef struct {
	Bool CPPM_Mode;
	Bool active_connection;
	uint16_t value[MAX_NUMBER_OF_INPUTS];
	uint16_t rssi;
	uint16_t rssi_period;
} Raw_External_Input_Type;

typedef enum {
	INPUT_CH1_CPPM = 0,
	INPUT_CH2_RSSI = 1,
	INPUT_CH3 = 2,
	INPUT_CH4 = 3,
	INPUT_CH5 = 4,
	INPUT_CH6 = 5
} Input_Capture_Channel;

/* Global functions */
void Input_CPPM_RSSI_Config(void);
void Input_PWM_Config(void);

#endif

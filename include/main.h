#include "stm32f4xx.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#ifndef NULL
#define NULL ((void*) 0)
#endif

void main(void);
void vTaskCode(void *);
void vTaskCode1(void *);

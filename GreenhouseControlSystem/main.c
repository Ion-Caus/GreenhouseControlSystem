/*
* main.c
* Author : ionc
*
*/

#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

#include <lora_driver.h>
#include <status_leds.h>

#include "upLinkHandler_LoraWAN.h"
#include "downLinkHandler.h"
#include "application.h"
#include "temp_hum.h"
#include "co2.h"
#include "weighted_average.h"
#include "moisture.h"

#include "config.h"
#include "lorawanConfig.h"

#include "thresholdConfiguration.h"
#include "sensorDataPackageHandler.h"

#include "buffersHandler.h"
#include "eventGroupsHandler.h"


extern MessageBufferHandle_t downlinkBuffer;

void structures_create() {
	// created the message buffer for window, upLink and downLink task
	buffersHandler_create();
	
	// creates the event group of the application and sensors tasks
	eventGroupsHandler_create();
	
	// creates the threshold mutex
	thresholdMutex_create();
}

void tasks_create() {
	application_task_create();
	tempHum_task_create();
	moisture_task_create();
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	// Create tasks
	tasks_create();
	
	// ===== BELOW IS LoRaWAN initialisation =====
	// Status LEDs driver
	status_leds_initialise(LEDS_STATUS_PRIORITY);
		
	// Initialize the LoRaWAN driver with a down-link buffer
	lora_driver_initialise(DOWNLINK_COM_PORT, downLinkBuffer);
	
	// Create UpLinkHandler and setup LoRaWAN
	upLinkHandler_task_create();
	
	// Create DownLinkTaskHandler 
	downLinkHandler_task_create();
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!! 
	
	 // mutex for accessing weighted average calculation by different tasks
	avg_calc_mutex = xSemaphoreCreateMutex();
	if((avg_calc_mutex)!=NULL){
		xSemaphoreGive((avg_calc_mutex));
	}		

	puts("Program Started!!\n");
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1) {}
}


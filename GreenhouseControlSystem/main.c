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

 // Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

#include "upLinkHandler_LoraWAN.h"
#include "downLinkHandler.h"
#include "application.h"
#include "temp_hum.h"

#include "payloadConfig.h"

#include "ThresholdConfiguration.h"
#include "sensorDataPackageHandler.h"


MessageBufferHandle_t windowBuffer;
MessageBufferHandle_t upLinkBuffer;
MessageBufferHandle_t downLinkBuffer;


/*-----------------------------------------------------------*/
void initBuffers() {
	windowBuffer =  xMessageBufferCreate( sizeof(measurements_t) * 2 );
	upLinkBuffer =  xMessageBufferCreate( sizeof(measurements_t) * 2 );
	downLinkBuffer = xMessageBufferCreate( sizeof(lora_driver_payload_t) * 2 );
}

void initThresholdMutex() {
	thresholdMutex_create();
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	// Initialize buffers for upLink and downLink Lora handler
	initBuffers();
	
	initThresholdMutex();
	
	// Creates tasks
	createApplicationTask();
	createTemperatureHumidityTask();
	
	
	// ===== BELOW IS LoRaWAN initialisation =====
	// Status LEDs driver
	status_leds_initialise(5); // Priority 5 for internal task
		
	// Initialize the LoRaWAN driver with a down-link buffer
	lora_driver_initialise(1, downLinkBuffer);
	
	// Create UpLinkHandler and setup LoRaWAN with priority 3
	upLinkHandler_task_init(3);
	
	// Create DownLinkTaskHandler 
	downLinkHandler_task_init();
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!! 
	
	puts("Program Started!!\n");
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1) {}
}


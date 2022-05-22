/*
* main.c
* Author : IHA
*
* Example main file including LoRaWAN setup
* Just for inspiration :)
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
#include "application.h"
#include "temp_hum.h"

#include "payloadConfig.h"

#include "ThresholdConfiguration.h"


MessageBufferHandle_t upLinkBuffer;
MessageBufferHandle_t downLinkBuffer;


/*-----------------------------------------------------------*/
void initBuffers() {
	upLinkBuffer =  xMessageBufferCreate( UPLINK_PAYLOAD_LENGHT * 2 );
	downLinkBuffer = xMessageBufferCreate( UPLINK_PAYLOAD_LENGHT * 2 );
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
		
	// Initialize the LoRaWAN driver without down-link buffer
	lora_driver_initialise(1, NULL);
	
	// Create LoRaWAN task and start it up with priority 3
	upLinkHandler_task_init(3);
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!! 
	
	puts("Program Started!!\n");
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1) {}
}


/*
 * downLinkHandler.c
 *
 * Created: 5/17/2022 9:52:38 AM
 *  Author: ionc
 */ 

#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>

#include "downLinkHandler.h"


#define DOWNLINK_HANDLER_TASK_DELAY_MS				(60L * 1000) // Check the buffer every 1 minute

#define DOWNLINK_HANDLER_TASK_STACK					( configMINIMAL_STACK_SIZE + 200 )
#define DOWNLINK_HANDLER_TASK_STACK_PRIORITY		( tskIDLE_PRIORITY + 2 )


extern MessageBufferHandle_t downLinkBuffer;

void downLinkHandler_task( void *pvParameters )
{
	
	TickType_t xLastWakeTime;
	
	const TickType_t xFrequency = pdMS_TO_TICKS(DOWNLINK_HANDLER_TASK_DELAY_MS);
	xLastWakeTime = xTaskGetTickCount();
	
	
	uint8_t payloadBuffer[sizeof(lora_driver_payload_t)] = {0};
	
	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		// receiving the payload from the downLink buffer
		// wait until is not empty
		xMessageBufferReceive(downLinkBuffer,
		(void*)payloadBuffer,
		sizeof(lora_driver_payload_t),
		portMAX_DELAY);
		
		printf("Received message from DownLinkBuffer\n");
		
		for (uint8_t i = 0; i < sizeof(lora_driver_payload_t); i++) {
			printf("%d, ", payloadBuffer[i]);
		}
		printf("\n");
		
		
		
		puts("Uploading Thresholds.\n");
		
	 
		for (uint8_t i = 0; i < ((lora_driver_payload_t*)payloadBuffer)->len; i++) {
			printf("%d, ", ((lora_driver_payload_t*)payloadBuffer)->bytes[i]);
		}
		printf("\n");
		// ToDo : call the config and set the thresholds
	}
}

void downLinkHandler_task_init()
{
	xTaskCreate(
	downLinkHandler_task
	,  "DownLinkHandler"  
	,  DOWNLINK_HANDLER_TASK_STACK  
	,  NULL
	,  DOWNLINK_HANDLER_TASK_STACK_PRIORITY 
	,  NULL );
}
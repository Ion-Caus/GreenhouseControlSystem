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
#include "ThresholdConfiguration.h"


#define DOWNLINK_HANDLER_TASK_DELAY_MS				(60L * 1000) // Check the buffer every 1 minute

#define DOWNLINK_HANDLER_TASK_STACK					( configMINIMAL_STACK_SIZE + 200 )
#define DOWNLINK_HANDLER_TASK_STACK_PRIORITY		( tskIDLE_PRIORITY + 2 )


extern MessageBufferHandle_t downLinkBuffer;

static lora_driver_payload_t loraPayload;

void downLinkHandler_task( void *pvParameters )
{
	
	TickType_t xLastWakeTime;
	
	const TickType_t xFrequency = pdMS_TO_TICKS(DOWNLINK_HANDLER_TASK_DELAY_MS);
	xLastWakeTime = xTaskGetTickCount();

	
	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		// receiving the payload from the downLink buffer
		// wait until is not empty
		xMessageBufferReceive(downLinkBuffer,
		(void*)&loraPayload,
		sizeof(lora_driver_payload_t),
		portMAX_DELAY);
		
		printf("Received message from DownLinkBuffer\n");
		
		for (uint8_t i = 0; i < loraPayload.len; i++) {
			printf("%d, ", loraPayload.bytes[i]);
		}
		printf("\n");
		
		puts("Setting the Thresholds.\n");
		int16_t tempMin = loraPayload.bytes[0] | loraPayload.bytes[1] << 8;
		int16_t tempMax = loraPayload.bytes[2] | loraPayload.bytes[3] << 8;
		
		uint16_t co2Min = loraPayload.bytes[4] | loraPayload.bytes[5] << 8;
		uint16_t co2Max = loraPayload.bytes[6] | loraPayload.bytes[7] << 8;
		
		printf("%d, %d\n", tempMin, tempMax);
		printf("%d, %d\n", co2Min, co2Max);
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
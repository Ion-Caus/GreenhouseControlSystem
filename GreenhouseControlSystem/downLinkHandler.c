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
#include "thresholdConfiguration.h"
#include "buffersHandler.h"

#include "config.h"

extern MessageBufferHandle_t downLinkBuffer;

static lora_driver_payload_t loraPayload;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency  = pdMS_TO_TICKS(DOWNLINK_HANDLER_TASK_DELAY_MS);

void downLinkHandler_task_run()
{
	xTaskDelayUntil( &xLastWakeTime, xFrequency );
	
	// receiving the payload from the downLink buffer
	// wait until is not empty
	size_t bytesReceived = xMessageBufferReceive(downLinkBuffer,
			(void*)&loraPayload,
			sizeof(lora_driver_payload_t),
			portMAX_DELAY);
	
	printf("Received message from DownLinkBuffer\n");
	
	#if DEV_ENV
		// TODO : delete in production
		for (uint8_t i = 0; i < loraPayload.len; i++) {
			printf("%d, ", loraPayload.bytes[i]);
		}
		printf("\n");
		// --------------------------
	#endif
	
	if (bytesReceived < 8) return;
	
	int16_t tempMin = loraPayload.bytes[1] | loraPayload.bytes[0] << 8;
	int16_t tempMax = loraPayload.bytes[3] | loraPayload.bytes[2] << 8;
	
	if (!tempMax) tempMax = TEMP_MAX_THRESHOLD;
	
	uint16_t co2Min = loraPayload.bytes[5] | loraPayload.bytes[4] << 8;
	uint16_t co2Max = loraPayload.bytes[7] | loraPayload.bytes[6] << 8;
	
	if (!co2Max) co2Max = CO2_MAX_THRESHOLD;
	
	puts("Updating the Thresholds.\n");
	
	// setting up the thresholds 
	thresholdMutex_setTempLower(tempMin);
	thresholdMutex_setTempUpper(tempMax);
	
	thresholdMutex_setCo2Lower(co2Min);
	thresholdMutex_setCo2Upper(co2Max);
	
}

void downLinkHandler_task( void *pvParameters )
{
	
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
		downLinkHandler_task_run();
	}
}

void downLinkHandler_task_create()
{
	xTaskCreate(
	downLinkHandler_task
	,  "DownLinkHandler"  
	,  DOWNLINK_TASK_STACK  
	,  NULL
	,  DOWNLINK_TASK_PRIORITY 
	,  NULL );
}
/*
 * Application.c
 *
 * Created: 04/05/2022 21.32.57
 *  Author: Deniss
 */ 


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include "event_groups.h"
#include "application.h"
#include "temp_hum.h"
#include "sensorDataPackageHandler.h"

#include "payloadConfig.h"

#define APPICATION_TASK_DELAY_MS				(3000UL) // same as Lora delay

static uint8_t counter = 0;


extern MessageBufferHandle_t upLinkBuffer; 

void initEventGroups(void){
	_measureEventGroup = xEventGroupCreate();
	_readingsReadyEventGroup = xEventGroupCreate();
	
}

void applicationTask(void* pvParameter){
	(void)pvParameter; //discarding parameters;
	
	puts("Application task started!\n");
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(APPICATION_TASK_DELAY_MS);

	xLastWakeTime = xTaskGetTickCount();
	 
	for(;;) {
		//Tells the Temperature & Humidity sensor to wake up and collect data
		xEventGroupSetBits(_measureEventGroup, BIT_TASK_TEMPHUM); 
		
		//Tells the CO2 sensor to wake up and collect data
		//xEventGroupSetBits(_measureEventGroup, BIT_TASK_CO2); 
		
		//Tells the moisture sensor to start measurements
		//printf("setting the bit");
		xEventGroupSetBits(_measureEventGroup, BIT_TASK_MOIST);
		
		uint8_t bits = BIT_TASK_TEMPHUM | BIT_TASK_MOIST; //| BIT_TASK_CO2;
		
		//wait for the tasks to return with their measurements and set their event group flags							
		xEventGroupWaitBits(_readingsReadyEventGroup, 
							bits,
							pdTRUE,
							pdTRUE,
							portMAX_DELAY
							); 
						
						
		//once the measure task are ready pause them
		xEventGroupClearBits(_measureEventGroup, bits);
	
		//getting the calculated temperature from the sensor
		int16_t measuredTemperature = getTemperature();
		
		//printing moisture of the first plant
		uint8_t* moistureArr = moisture_getMoistures();
		
		//printf("giving values: ");
		//for (int i = 0; i < 6; i++) {
		//	printf("%d, ",moistureArr[i]);
		//}
		//printf("measurement number: %d\n",++counter);
		
		//providing data for the Lora payload
		setTemperature(measuredTemperature);
	
		//getting Lora payload package
		uint8_t* payload = getArrPayload();
		puts("Application task got the payload\n");
		
		if (!xMessageBufferIsEmpty(upLinkBuffer)) {
			// reset the buffer to override the payload
			xMessageBufferReset(upLinkBuffer);
		}
	
		//sending the payload to upLink buffer
		size_t sentBytes = xMessageBufferSend(upLinkBuffer,
			(void*)payload,
			UPLINK_PAYLOAD_LENGHT,
			portMAX_DELAY);
		
		printf("Sent payload to upLink buffer, sent bytes =%d\n", sentBytes);
	
		xTaskDelayUntil( &xLastWakeTime, xFrequency);	
	}
	

}

void createApplicationTask(void){
	initEventGroups();
	
	xTaskCreate(
	applicationTask,
	"Application Task",
	APPLICATION_TASK_STACK,
	NULL,
	APPLICATION_TASK_PRIORITY,
	NULL
	);
}
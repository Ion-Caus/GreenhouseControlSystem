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

#define APPICATION_TASK_DELAY_MS				(100000UL)


extern MessageBufferHandle_t windowBuffer; 
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
		
		uint8_t bits = BIT_TASK_TEMPHUM; //| BIT_TASK_CO2;
		
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
		
		//getting the calculated temperature from the sensor
		uint16_t measuredHumidity = getHumidity();
		
		//providing data for the sensor package
		sensorDataPackage_reset();
		setTemperature(measuredTemperature);
		setHumidity(measuredHumidity);
	
		//getting measurements data package
		measurements_t package = getSensorData();
		puts("Application task got the data package\n");
		
		if (!xMessageBufferIsEmpty(upLinkBuffer)) {
			// reset the buffer to override the payload
			xMessageBufferReset(upLinkBuffer);
		}
	
		//sending the payload to upLink buffer
		size_t sentBytes = xMessageBufferSend(upLinkBuffer,
			(void*)&package,
			sizeof(measurements_t),
			portMAX_DELAY);
		
		printf("Sent data package to upLink buffer, sent bytes =%d\n", sentBytes);
		
		//sending the payload to upLink buffer
		sentBytes = xMessageBufferSend(windowBuffer,
		(void*)&package,
		sizeof(measurements_t),
		portMAX_DELAY);
		
		printf("Sent data package to window buffer, sent bytes =%d\n", sentBytes);
	
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
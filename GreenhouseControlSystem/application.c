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

#include "application.h"
#include "temp_hum.h"
#include "moisture.h"
#include "sensorDataPackageHandler.h"
#include "eventGroupsHandler.h"
#include "buffersHandler.h"
#include "co2.h"
#include "config.h"

extern EventGroupHandle_t measureEventGroup;
extern EventGroupHandle_t readingsReadyEventGroup;

extern MessageBufferHandle_t windowBuffer; 
extern MessageBufferHandle_t  upLinkBuffer; 

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = pdMS_TO_TICKS(APPICATION_TASK_DELAY_MS);

void application_task_run() 
{
	//Tells the Temperature & Humidity sensor to wake up and collect data
	xEventGroupSetBits(measureEventGroup, BIT_TASK_TEMPHUM);
	
	//Tells the CO2 sensor to wake up and collect data
	xEventGroupSetBits(measureEventGroup, BIT_TASK_CO2);
	
	//Tells the Moisture sensor to wake up and collect data
	xEventGroupSetBits(measureEventGroup, BIT_TASK_MOIST);
	
	uint8_t bits = BIT_TASK_TEMPHUM | BIT_TASK_CO2; //| BIT_TASK_MOIST 
	
	//wait for the tasks to return with their measurements and set their event group flags
	xEventGroupWaitBits(readingsReadyEventGroup,
		bits,
		pdTRUE,
		pdTRUE,
		APPICATION_TASK_DELAY_MS
		);
	
	//once the measure tasks are ready pause them
	xEventGroupClearBits(measureEventGroup, bits);
	
	//getting the calculated temperature from the sensor
	uint8_t status = 0;
	if(tempHum_getStatusTemperature()) status |= TEMP_STATUS;
	int16_t measuredTemperature = tempHum_getTemperature();
		
	//getting the calculated temperature from the sensor
	if(tempHum_getStatusHumidity()) status |= HUM_STATUS;
	uint16_t measuredHumidity = tempHum_getHumidity();
	
	//getting moisture array from sensor
	if(moisture_getStatus()) status |= MOISTURE_STATUS;
	uint8_t* measuredMoisture = moisture_getMoistures();
	
	//getting co2 from sensor
	if(co2_getCo2()) status |= CO2_STATUS;
	uint16_t measuredCo2 = co2_getCo2();
	
	//providing data for the sensor package
	sensorDataPackage_reset();
	sensorDataPackage_setTemperature(measuredTemperature);
	sensorDataPackage_setHumidity(measuredHumidity);
	sensorDataPackage_setCO2(measuredCo2);
	sensorDataPackage_setMoistures(measuredMoisture);
	sensorDataPackage_setStatus(status);
	
	//getting measurements data package
	measurements_t package = sensorDataPackage_getSensorData();
	puts("Application task got the data package\n");
	
	if (!xMessageBufferIsEmpty(upLinkBuffer)) {
		// reset the buffer to override the payload
		xMessageBufferReset(upLinkBuffer);
	}
	
	//sending the package to upLink buffer
	size_t sentBytes = xMessageBufferSend(upLinkBuffer,
		(void*)&package,
		sizeof(measurements_t),
		portMAX_DELAY);

	printf("Sent data package to upLink buffer, sent bytes =%d\n", sentBytes);
	
	//sending the package to window buffer
	sentBytes = xMessageBufferSend(windowBuffer,
		(void*)&package,
		sizeof(measurements_t),
		0);								// DELAY SET TO ZERO
	
	printf("Sent data package to window buffer, sent bytes =%d\n", sentBytes);
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency);
}

void application_task(void* pvParameter)
{
	(void)pvParameter; //discarding parameters;
	
	puts("Application task started!\n");

	xLastWakeTime = xTaskGetTickCount();
	 
	for(;;) {
		application_task_run();
	}
}

void application_task_create(void){
	xTaskCreate(
		application_task,
		"Application Task",
		APPLICATION_TASK_STACK,
		NULL,
		APPLICATION_TASK_PRIORITY,
		NULL
	);
}
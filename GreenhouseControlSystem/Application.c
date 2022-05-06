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
#include "temperature.h"
#include "sensorDataPackageHandler.h"

#define TEMP_DELAY_MS				(1000 * 3)

void initEventGroups(void){
	_measureEventGroup = xEventGroupCreate();
	_readingsReadyEventGroup = xEventGroupCreate();
	
}

void applicationTask(void* pvParameter){
	(void)pvParameter; //discarding parameters;
	
	 TickType_t xLastWakeTime;
	 const TickType_t xFrequency = TEMP_DELAY_MS/portTICK_PERIOD_MS;

	 xLastWakeTime = xTaskGetTickCount();
	 
	
	for( ;; ){
		xEventGroupSetBits(_measureEventGroup, BIT_TASK_TEMPHUM); //Tells the Temperature & Humidity sensor to wake up and collect data
		//xEventGroupSetBits(_measureEventGroup, BIT_TASK_CO2); //Tells the CO2 sensor to wake up and collect data
	
		
		int bits = BIT_TASK_TEMPHUM;
		//| BIT_TASK_CO2;
	
		puts("waiting for measurements");
		xEventGroupWaitBits(_readingsReadyEventGroup, 
							bits,
							pdTRUE,
							pdTRUE,
							portMAX_DELAY
							); //wait for the tasks to return with their measurements and set their event group flags 
						
						
		//once the task is ready 
		//pause that task
		xEventGroupClearBits(_measureEventGroup, bits);
	
		//getting the calculated temperature from the sensor
		int measuredTemperature = getTemperature();
		
		//providing data for the Lora payload
		setTemperature(measuredTemperature);
	
		//getting Lora payload package
		//lora_driver_payload_t payload = getLoRaPayload(LORA_PORTNO);
	
		xTaskDelayUntil( &xLastWakeTime, xFrequency);	
	}
	

}

void createApplicationTask(void){
	
	puts("created the application task!");
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
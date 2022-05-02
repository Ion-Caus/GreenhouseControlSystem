/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc
 */ 
#include "temperature.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>

#include <stdio_driver.h>


#define TEMP_DELAY_MS				(200)

// move to .h
#define TEMPERATURE_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define TEMPERATURE_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )

#define MIN_TEMPERATURE				(-20)
#define MAX_TEMPERATUE				(40)

#define TEMPERATURE_ARRAY_SIZE		(10)
static  int16_t weightedTemperature = MIN_TEMPERATURE;

int16_t getTemperature() {
	return weightedTemperature;
}


void initTempDriver() {
	hih8120_driverReturnCode_t returnCode = hih8120_initialise();
	
	if (returnCode != HIH8120_OK) {
		puts("Temperature driver failed to initialize.\n");
		return;
	}
	
	puts("Temperature driver initialized.\n");
}


void temperatureTask(void* pvParameter) {
	 (void) pvParameter;
	 
	 TickType_t xLastWakeTime;
	 const TickType_t xFrequency = TEMP_DELAY_MS/portTICK_PERIOD_MS;

	 xLastWakeTime = xTaskGetTickCount();
	 
	 int temperatureArray[TEMPERATURE_ARRAY_SIZE];
	 
	 uint8_t counter = 0; 
	 for (;;)
	 { 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(50) );
		 
		 
		 hih8120_driverReturnCode_t returnCode;
		 if (HIH8120_OK != (returnCode = hih8120_wakeup())) {
			 printf("Temperature Driver failed to wake up, %d\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(100) );
		 
		 if (HIH8120_OK != (returnCode = hih8120_measure())) {
			 printf("Temperature Driver failed to measure, %d\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(10) );
		 
		 int16_t temperature = hih8120_getTemperature_x10();
		 if (temperature < MIN_TEMPERATURE*10 || temperature > MAX_TEMPERATUE*10)
		 {
			 // if temperature exceeds realistic values
			 continue;
		 }
			
		 
		 temperatureArray[counter] = temperature;
		 printf("Temperature : %d\n", temperatureArray[counter]);
		 
		 
		 if (++counter < TEMPERATURE_ARRAY_SIZE) {
			xTaskDelayUntil( &xLastWakeTime, xFrequency );
			continue;
		 }
		 
		 counter = 0;
		 
		 // temp not ready
		 
		 // calculation of weighted average temperature 
		 weightedTemperature = temperatureArray[0];
		 for (uint8_t i = 1; i < TEMPERATURE_ARRAY_SIZE; i++) {
			 weightedTemperature = weightedTemperature*0.75 + temperatureArray[i]*0.25;
		 }
		 
		 printf("Weighted average temperature: %d\n", weightedTemperature);
		 // ready to be taken
		
	 }
}


void createTemperatureTask(void) {
	
	initTempDriver();
	
	xTaskCreate(
		temperatureTask,		// function task name
		"Temperature Task",		// task name
		TEMPERATURE_TASK_STACK,
		NULL,
		TEMPERATURE_TASK_PRIORITY,
		NULL
	);
}

void tempeperature_destory() {
	hih8120_destroy();
}

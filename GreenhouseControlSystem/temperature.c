/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc
 */ 
#include "temperature.h"
#include "application.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>

#include <stdio_driver.h>

#define TEMP_DELAY_MS				(200)

// move to .h
#define TEMPERATURE_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define TEMPERATURE_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )

#define MIN_TEMPERATURE				(-20 * 10)
#define MAX_TEMPERATUE				(40 * 10)

#define TEMPERATURE_ARRAY_SIZE		(10)
static int16_t weightedTemperature;


int16_t getTemperature() {
	return weightedTemperature;
}

int16_t calculateWeightedAverage(int16_t array[], uint8_t size) {
	int16_t weightedAverage = array[0];
	for (uint8_t i = 1; i < size; i++) {
		weightedAverage = ceil( weightedAverage*0.75 + array[i]*0.25 ); // round up
	}
	
	return weightedAverage;
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
	 
	 int16_t temperatureArray[TEMPERATURE_ARRAY_SIZE];
	 
	 uint8_t counter = 0; 
	 
	 
	 //wait for 
	 for (;;)
	 { 
		  xEventGroupWaitBits(_measureEventGroup,
		  BIT_TASK_TEMPHUM,
		  pdFALSE,
		  pdTRUE,
		  portMAX_DELAY
		  );
		 
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
		 if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATUE)
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
		 weightedTemperature = calculateWeightedAverage(temperatureArray, TEMPERATURE_ARRAY_SIZE);
		 printf("Weighted average temperature: %d\n", weightedTemperature);
		 // temp ready to be taken
		 xEventGroupSetBits(_readingsReadyEventGroup, BIT_TASK_TEMPHUM);
		 xTaskDelayUntil( &xLastWakeTime, xFrequency);
		
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

/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc & alex
 */ 
#include "temp_hum.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>

#include <stdio_driver.h>


#define TEMP_HUM_DELAY_MS			(200)
#define RETRIES_LEFT				(5)

// move to .h
#define TEMP_HUM_TASK_STACK			( configMINIMAL_STACK_SIZE )
#define TEMP_HUM_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#define MIN_TEMPERATURE				(-20 * 10)
#define MAX_TEMPERATUE				(40 * 10)

#define MAX_HUMIDITY				(1000) // 100%

#define TEMP_HUM_ARRAY_SIZE			(10)

static int16_t weightedTemperature;
static uint16_t weightedHumidity;

int16_t getTemperature() {
	return weightedTemperature;
}

uint16_t getHumidity() {
	return weightedHumidity;
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
	
	for (int i = 0; i < RETRIES_LEFT; i++) {
		if (returnCode != HIH8120_OK) {
			puts("Temperature/humidity driver failed to initialize.\n");
			return;
		}
	}
	
	puts("Temperature/humidity driver initialized.\n");
}

// task that awakens the driver and gets measurements of temperature and humidity
void temperatureHumidityTask(void* pvParameter) {
	 (void) pvParameter;
	 
	 TickType_t xLastWakeTime;
	 const TickType_t xFrequency = TEMP_HUM_DELAY_MS/portTICK_PERIOD_MS;

	 xLastWakeTime = xTaskGetTickCount();
	 
	 int16_t temperatureArray[TEMP_HUM_ARRAY_SIZE];
	 int16_t humidityArray[TEMP_HUM_ARRAY_SIZE];
	 
	 uint8_t counter = 0; 
	 for (;;)
	 { 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(50) );
		 
		 
		 hih8120_driverReturnCode_t returnCode;
		 if (HIH8120_OK != (returnCode = hih8120_wakeup())) {
			 printf("Temperature/humidity driver failed to wake up, %d\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(100) );
		 
		 if (HIH8120_OK != (returnCode = hih8120_measure())) {
			 printf("Temperature/humidity driver failed to measure, %d\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(10) );
		 
		 int16_t temperature = hih8120_getTemperature_x10();
		 uint16_t humidity = hih8120_getHumidityPercent_x10();
		 
		 if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATUE || humidity > MAX_HUMIDITY)
		 {
			 // if temperature or humidity exceeds realistic values
			 continue;
		 }
			
		 
		 temperatureArray[counter] = temperature;
		 printf("Temperature : %d\n", temperatureArray[counter]);
		 
		 humidityArray[counter] = humidity;
		 printf("Humidity : %d\n", humidityArray[counter]);
		 
		 
		 if (++counter < TEMP_HUM_ARRAY_SIZE) {
			xTaskDelayUntil( &xLastWakeTime, xFrequency );
			continue;
		 }
		 
		 counter = 0;
		 
		 // temp_hum not ready
		 
		 // calculation of weighted averages
		 weightedTemperature = calculateWeightedAverage(temperatureArray, TEMP_HUM_ARRAY_SIZE);
		 printf("Weighted average temperature: %d\n", weightedTemperature);
		 // temp ready to be taken
		 
		 weightedHumidity = calculateWeightedAverage(humidityArray, TEMP_HUM_ARRAY_SIZE);
		 printf("Weighted average humidity: %d\n", weightedHumidity);
		// humidity ready to be taken
	 }
}


void createTemperatureHumidityTask(void) {
	
	initTempDriver();
	
	xTaskCreate(
		temperatureHumidityTask,			// function task name
		"Temperature/humidity Task",		// task name
		TEMP_HUM_TASK_STACK,
		NULL,
		TEMP_HUM_TASK_PRIORITY,
		NULL
	);
}

void tempeperature_destroy() {
	hih8120_destroy();
}

/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc & alex
 */ 
#include "temp_hum.h"
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

#define TEMP_HUM_DELAY_MS			(200)
#define MAX_RETRIES					(5)

// move to .h
#define TEMP_HUM_TASK_STACK			( configMINIMAL_STACK_SIZE )
#define TEMP_HUM_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#define MIN_TEMPERATURE				(-20 * 10)
#define MAX_TEMPERATUE				(40 * 10)
#define MAX_HUMIDITY				(1000) // 100%

#define TEMP_HUM_ARRAY_SIZE			(10)

static int16_t weightedTemperature;
static uint16_t weightedHumidity;

extern EventGroupHandle_t _measureEventGroup;
extern EventGroupHandle_t _readingsReadyEventGroup;


int16_t getTemperature() {
	return weightedTemperature;
}

uint16_t getHumidity() {
	return weightedHumidity;
}

int16_t calculateWeightedAverage(int16_t array[], uint8_t size) {
	// calculation the weighted average for an array of measurements 
	int16_t weightedAverage = array[0];
	for (uint8_t i = 1; i < size; i++) {
		weightedAverage = ceil( weightedAverage*0.75 + array[i]*0.25 ); // round up
	}
	
	return weightedAverage;
}


void initTempDriver() {
	hih8120_driverReturnCode_t returnCode = hih8120_initialise();
	
	for (int i = 0; i < MAX_RETRIES; i++) {
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
	 
	 // temporary arrays to store measurements to calculate a weighted average
	 int16_t temperatureArray[TEMP_HUM_ARRAY_SIZE];
	 int16_t humidityArray[TEMP_HUM_ARRAY_SIZE];
	 
	 uint8_t index = 0; 
	 for (;;)
	 { 
		 // waiting for the application task to set the bit as ready
		  xEventGroupWaitBits(_measureEventGroup,
			  BIT_TASK_TEMPHUM,
			  pdFALSE,
			  pdTRUE,
			  portMAX_DELAY
			  );
		 
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
		 
		 // get the temperature value
		 int16_t temperature = hih8120_getTemperature_x10();
		 uint16_t humidity = hih8120_getHumidityPercent_x10();
		 
		 if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATUE)
		 {
			 // if temperature exceeds realistic values
			 continue;
		 }
		 
		 if (humidity > MAX_HUMIDITY) {
			 continue; // humidity exceeds the norm
		 }
			
		 // populate the array with samples
		 temperatureArray[index] = temperature;
		 printf("Temperature : %d\n", temperatureArray[index]);
		 
		 humidityArray[index] = humidity;
		 printf("Humidity : %d\n", humidityArray[index]);
		 
		 
		 // fill the array to be able to calculate the weighted average
		 if (++index < TEMP_HUM_ARRAY_SIZE) {
			xTaskDelayUntil( &xLastWakeTime, xFrequency );
			continue;
		 }
		  
		 // reset index 
		 index = 0;
		 
		 		 
		 // calculation of weighted averages
		 weightedTemperature = calculateWeightedAverage(temperatureArray, TEMP_HUM_ARRAY_SIZE);
		 printf("Weighted average temperature: %d\n", weightedTemperature);
		 
		 weightedHumidity = calculateWeightedAverage(humidityArray, TEMP_HUM_ARRAY_SIZE);
		 printf("Weighted average humidity: %d\n", weightedHumidity);
		 
		 
		  // announce application task that the data is ready to be taken
		  xEventGroupSetBits(_readingsReadyEventGroup, BIT_TASK_TEMPHUM);
		  
		  
		  xTaskDelayUntil( &xLastWakeTime, xFrequency);
	 }
}


void createTemperatureHumidityTask(void) {
		 
	initTempDriver();
	
	xTaskCreate(
		temperatureHumidityTask,		// function task name
		"Temperature Task",				// task name
		TEMP_HUM_TASK_STACK,
		NULL,
		TEMP_HUM_TASK_PRIORITY,
		NULL
	);

}

 
void temperatureHumidity_destroy() {
	hih8120_destroy();
}

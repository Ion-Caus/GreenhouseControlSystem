/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc & alex
 */ 

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>
#include <stdio_driver.h>

#include "temp_hum.h"
#include "application.h"
#include "eventGroupsHandler.h"
#include "weighted_average.h"
#include "config.h"


extern EventGroupHandle_t measureEventGroup;
extern EventGroupHandle_t readingsReadyEventGroup;

static int16_t weightedTemperature;
static uint16_t weightedHumidity;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = TEMP_HUM_DELAY_MS/portTICK_PERIOD_MS;


int16_t tempHum_getTemperature() {
	return weightedTemperature;
}

uint16_t tempHum_getHumidity() {
	return weightedHumidity;
}

static inline void tempHum_wakeupAndMeasure() {
	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

	hih8120_driverReturnCode_t returnCode;
	if (HIH8120_OK != (returnCode = hih8120_wakeup())) {
		printf("Temperature/humidity driver failed to wake up, %d\n", returnCode);
		return;
	}

	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));

	if (HIH8120_OK != (returnCode = hih8120_measure())) {
		printf("Temperature/humidity driver failed to measure, %d\n", returnCode);
		return;
	}

	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
}


void tempHum_task_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index) 
{
	// waiting for the application task to set the bit as ready
	xEventGroupWaitBits(measureEventGroup,
		BIT_TASK_TEMPHUM,
		pdFALSE,
		pdTRUE,
		portMAX_DELAY
		);

	// wake up the driver 
	// measure the temperature and humidity
	tempHum_wakeupAndMeasure();
	
	// get the temperature and humidity values
	int16_t temperature = hih8120_getTemperature_x10();
	uint16_t humidity = hih8120_getHumidityPercent_x10();
	
	if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATUE)
	{
		return; // temperature exceeds realistic values
	}
	
	if (humidity >= MAX_HUMIDITY) 
	{
		return; // humidity exceeds 99.9%
	}
	
	// populate the array with samples
	temperatureArray[(*index)] = temperature;
	humidityArray[(*index)] = humidity;
	#if DEV_ENV
		printf("Temperature : %d\n", temperatureArray[(*index)]);
		printf("Humidity : %d\n", humidityArray[(*index)]);
	#endif
	
	
	// arrays must be full before calculating the weighted average
	if (++(*index) < TEMP_HUM_ARRAY_SIZE) {
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		return;
	}
	
	// reset index
	*index = 0;
	
	
	// calculation of weighted averages
	weightedTemperature = calculateWeightedAverage(temperatureArray, TEMP_HUM_ARRAY_SIZE);
	printf("Weighted average temperature: %d\n", weightedTemperature);
	
	weightedHumidity = calculateWeightedAverage(humidityArray, TEMP_HUM_ARRAY_SIZE);
	printf("Weighted average humidity: %d\n", weightedHumidity);
	
	
	// announce application task that the data is ready to be taken
	xEventGroupSetBits(readingsReadyEventGroup, BIT_TASK_TEMPHUM);
	
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency);
}


void tempHum_task(void* pvParameter) {
	 (void) pvParameter;
	 
	 xLastWakeTime = xTaskGetTickCount();
	 
	 // arrays to store measurements to calculate a weighted average
	 int16_t temperatureArray[TEMP_HUM_ARRAY_SIZE];
	 int16_t humidityArray[TEMP_HUM_ARRAY_SIZE];
	 
	 uint8_t index = 0; 
	 
	 
	 for (;;)
	 { 
		 tempHum_task_run(temperatureArray, humidityArray, &index);
	 }
}


void tempHum_initDriver() {
	hih8120_driverReturnCode_t returnCode;

	for (int i = 0; i < MAX_RETRIES; i++) {
		if (HIH8120_OK == ( returnCode = hih8120_initialise()) ) {
			puts("Temperature/humidity driver initialized.\n");
			return;
		}
	}
	puts("Temperature/humidity driver failed to initialize.\n");
}


void tempHum_task_create(void) {
		 
	tempHum_initDriver();
	
	xTaskCreate(
		tempHum_task,								// function task name
		"Temperature and Humidity Task",			// task name
		TEMP_HUM_TASK_STACK,
		NULL,
		TEMP_HUM_TASK_PRIORITY,
		NULL
	);

}

 
void tempHum_driver_destroy() {
	//hih8120_destroy();
}

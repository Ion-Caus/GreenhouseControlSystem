/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc & alex
 */ 
#include "temp_hum.h"
#include "application.h"
#include "weighted_average.h"

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
#define MAX_TEMPERATUE				(60  * 10)
#define MAX_HUMIDITY				(100 * 10) // 100%

#define TEMP_HUM_ARRAY_SIZE			(10)


extern EventGroupHandle_t _measureEventGroup;
extern EventGroupHandle_t _readingsReadyEventGroup;

static int16_t weightedTemperature;
static uint16_t weightedHumidity;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = TEMP_HUM_DELAY_MS/portTICK_PERIOD_MS;

int16_t getTemperature() {
	return weightedTemperature;
}

uint16_t getHumidity() {
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

// covered by tests
inline void tempHumTask_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index) {
	// waiting for the application task to set the bit as ready
	xEventGroupWaitBits(_measureEventGroup,
		BIT_TASK_TEMPHUM,
		pdFALSE,
		pdTRUE,
		portMAX_DELAY
		);

	// wake up and measure the temperature and humidity
	tempHum_wakeupAndMeasure();
	
	// get the temperature value
	int16_t temperature = hih8120_getTemperature_x10();
	uint16_t humidity = hih8120_getHumidityPercent_x10();
	
	if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATUE)
	{
		// if temperature exceeds realistic values
		return;
	}
	
	if (humidity > MAX_HUMIDITY) {
		return; // humidity exceeds the norm
	}
	
	// populate the array with samples
	temperatureArray[(*index)] = temperature;
	printf("Temperature : %d\n", temperatureArray[(*index)]);
	
	humidityArray[(*index)] = humidity;
	printf("Humidity : %d\n", humidityArray[(*index)]);
	
	
	// fill the array to be able to calculate the weighted average
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
	xEventGroupSetBits(_readingsReadyEventGroup, BIT_TASK_TEMPHUM);
	
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency);
}


// task that awakens the driver and gets measurements of temperature and humidity
void temperatureHumidityTask(void* pvParameter) {
	 (void) pvParameter;
	 
	 xLastWakeTime = xTaskGetTickCount();
	 
	 // temporary arrays to store measurements to calculate a weighted average
	 int16_t temperatureArray[TEMP_HUM_ARRAY_SIZE];
	 int16_t humidityArray[TEMP_HUM_ARRAY_SIZE];
	 
	 uint8_t index = 0; 
	 for (;;)
	 { 
		 tempHumTask_run(temperatureArray, humidityArray, &index);
	 }
}

// covered by tests
void initTempHumDriver() {
	hih8120_driverReturnCode_t returnCode;

	for (int i = 0; i < MAX_RETRIES; i++) {
		if (HIH8120_OK == ( returnCode = hih8120_initialise()) ) {
			puts("Temperature/humidity driver initialized.\n");
			return;
		}
	}
	puts("Temperature/humidity driver failed to initialize.\n");
}


void createTemperatureHumidityTask(void) {
		 
	initTempHumDriver();
	
	xTaskCreate(
		temperatureHumidityTask,					// function task name
		"Temperature and Humidity Task",			// task name
		TEMP_HUM_TASK_STACK,
		NULL,
		TEMP_HUM_TASK_PRIORITY,
		NULL
	);

}

 
void temperatureHumidity_destroy() {
	//hih8120_destroy();
}

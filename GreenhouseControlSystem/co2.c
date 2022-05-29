/*
 * co2.c
 *
 * Created: 18/05/2022 19.39.45
 *  Author: alex, deniss, ion
 */ 

#include "co2.h"
#include "application.h"
#include "eventGroupsHandler.h"
#include "weighted_average.h"
#include "config.h"
#include "sensorConfig.h"

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <mh_z19.h>
#include <stdio_driver.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern EventGroupHandle_t measureEventGroup;
extern EventGroupHandle_t readingsReadyEventGroup;


static uint16_t weightedCo2;
static bool isWorking;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = CO2_DELAY_MS/portTICK_PERIOD_MS;


uint16_t co2_getCo2() {
	return weightedCo2;
}

static inline void co2_wakeupAndMeasure(uint16_t* co2)
{
	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
	
	mh_z19_returnCode_t returnCode;
	if ((returnCode = mh_z19_takeMeassuring()) != MHZ19_OK)
	{
		#if DEV_ENV
			printf("Co2 driver failed to take the measurement: %d\n", returnCode);
		#endif
		isWorking = false;
		return;
	}

	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));

	if( (returnCode = mh_z19_getCo2Ppm(co2)) != MHZ19_OK )
	{
		#if DEV_ENV
			printf("Failed to retrieve the measurement of Co2: %d\n", returnCode);
		#endif
		isWorking = false;
		return;
	}
	isWorking = true;
	xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
}

void co2_task_run(int16_t* co2Array, uint8_t* index) 
{
	xEventGroupWaitBits(measureEventGroup,
		BIT_TASK_CO2,
		pdFALSE, //clear the bit so measurement will happen just after someone request it again
		pdTRUE,
		portMAX_DELAY
	);
	
	uint16_t co2 = 0;	
	
	co2_wakeupAndMeasure(&co2);
	
	if (co2 > MAX_CO2 || co2 == 0) 
	{
		return; // if co2 exceeds the norm
	}
	
	// populate the array with samples
	co2Array[*index] = co2;
	#if DEV_ENV
	printf("Co2 : %d\n", co2Array[*index]);
	#endif
	
	// fill the array to be able to calculate the weighted average
	if (++(*index) < CO2_ARRAY_SIZE) {
		xTaskDelayUntil( &xLastWakeTime, 200 );
		return;
	}
	
	// reset index
	*index = 0;
	
	
	// calculation of weighted average
	weightedCo2 = weightedAverage_calculate(co2Array, CO2_ARRAY_SIZE);
	printf("Weighted average co2: %d\n", weightedCo2);
	
	// data ready to take
	xEventGroupSetBits(readingsReadyEventGroup, BIT_TASK_CO2);

	xTaskDelayUntil(&xLastWakeTime, xFrequency);
}


void _co2Task(void* pvParameter) {
	(void) pvParameter;

	xLastWakeTime = xTaskGetTickCount();

	 // array of measurements for the calculation of the weighted average
	int16_t co2Array[CO2_ARRAY_SIZE];


	uint8_t index = 0;
	
	for (;;) {
		co2_task_run(co2Array, &index);
	}
}

bool co2_getStatus() {
	return isWorking;
}

void co2_initDriver() {
	mh_z19_initialise(ser_USART3);			
	puts("CO2 driver initialized.\n");
}


void co2_task_create(void) {
	
	co2_initDriver();
	
	xTaskCreate(
		_co2Task,
		"Co2 Task",
		CO2_TASK_STACK,
		NULL,
		CO2_TASK_PRIORITY,
		NULL
		);
}
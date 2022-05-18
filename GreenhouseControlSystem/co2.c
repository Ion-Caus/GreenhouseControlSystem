/*
 * co2.c
 *
 * Created: 18/05/2022 19.39.45
 *  Author: alex
 */ 

#include "co2.h"
#include "application.h"
#include "weighted_average.h"

#include <ATMEGA_FreeRTOS.h>
#include <mh_z19.h>
#include <stdio_driver.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CO2_DELAY_MS			( 400 )
#define CO2_TASK_STACK			( configMINIMAL_STACK_SIZE )
#define CO2_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#define CO2_ARRAY_SIZE			( 10 )

static uint16_t weightedCo2;


uint16_t getCo2() {
	return weightedCo2;
}

// initialize the driver
void initCo2Driver() {
	mh_z19_initialise(ser_USART3);
	puts("Co2 driver initialized!\n");
}


void co2Task(void* pvParameter) {
	(void) pvParameter;

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = CO2_DELAY_MS/portTICK_PERIOD_MS;
	xLastWakeTime = xTaskGetTickCount();

	// temp array for weighted average calculation
	int16_t co2Array[CO2_ARRAY_SIZE];


	uint8_t index = 0;
	uint16_t co2;
	
	for (;;) {
		
		xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

		mh_z19_returnCode_t returnCode;
		if ((returnCode = mh_z19_takeMeassuring()) != MHZ19_OK) {
			printf("Co2 driver failed to take the measurement: %d\n", returnCode);
			continue;
		}

		 xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));

		if((returnCode = mh_z19_getCo2Ppm(&co2)) != MHZ19_OK) {
			printf("Failed to retrieve the measurement of Co2: %d\n", returnCode);
			continue;
		}
		
		xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
		
		// array with co2 measurements
		co2Array[index] = co2;
		printf("Co2 : %d\n", co2Array[index]);
		
		 // fill the array to be able to calculate the weighted average
		 if (++index < CO2_ARRAY_SIZE) {
			 xTaskDelayUntil( &xLastWakeTime, xFrequency );
			 continue;
		 }
		 
		 // reset index
		 index = 0;
		 
		 
		 // calculation of weighted average
		 weightedCo2 = calculateWeightedAverage(co2Array, CO2_ARRAY_SIZE);
		 printf("Weighted average co2: %d\n", weightedCo2);

		 xTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}


void createCo2Task(void) {
	
	initCo2Driver();
	
	xTaskCreate(
		co2Task,
		"Co2 Task",
		CO2_TASK_STACK,
		NULL,
		CO2_TASK_PRIORITY,
		NULL
		);
}
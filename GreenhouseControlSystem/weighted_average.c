/*
 * weighted_average.c
 *
 * Created: 18/05/2022 22.01.08
 *  Author: alex & ionc
 */ 

#include <ATMEGA_FreeRTOS.h>
#include <math.h>
#include <stdint.h>
#include <semphr.h>

#include "weighted_average.h"

static SemaphoreHandle_t avg_calc_mutex;

void weightedAverage_createMutex(void) {
	avg_calc_mutex = xSemaphoreCreateMutex();
	
	if (avg_calc_mutex != NULL)
	{
		xSemaphoreGive(avg_calc_mutex);
	}	
}


int16_t weightedAverage_calculate(int16_t arr[], uint8_t size) {
	// calculate the weighted average for an array of measurements
	
	// block weighted average calculation for other tasks while one task is calculating
	xSemaphoreTake(avg_calc_mutex, portMAX_DELAY);

	int16_t weightedAverage = arr[0];
	for (uint8_t i = 1; i < size; i++) {
		weightedAverage = ceil( weightedAverage*0.75 + arr[i]*0.25 ); // round up
	}
		
	// release the lock for other tasks
	xSemaphoreGive(avg_calc_mutex);
	return weightedAverage;
}

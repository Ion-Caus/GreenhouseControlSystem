/*
 * weighted_average.c
 *
 * Created: 18/05/2022 22.01.08
 *  Author: slip1
 */ 

#include "weighted_average.h"
#include <math.h>
#include <stdint.h>


int16_t calculateWeightedAverage(int16_t array[], uint8_t size) {
	// calculation the weighted average for an array of measurements
	int16_t weightedAverage = array[0];
	for (uint8_t i = 1; i < size; i++) {
		weightedAverage = ceil( weightedAverage*0.75 + array[i]*0.25 ); // round up
	}
	
	return weightedAverage;
}
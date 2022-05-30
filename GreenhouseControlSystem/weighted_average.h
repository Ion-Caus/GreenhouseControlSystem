/*
 * weighted_average.h
 *
 * Created: 18/05/2022 21.59.14
 *  Author: alex
 */ 


#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>
#include <semphr.h>


/**
 * Create the semaphore mutex, so 
 * that only one task is using it
 */
void weightedAverage_createMutex(void);

/**
 * Calculates the weighted average for an array of measurements 
 */
int16_t weightedAverage_calculate(int16_t array[], uint8_t size);

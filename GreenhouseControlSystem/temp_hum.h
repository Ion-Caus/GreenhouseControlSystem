#pragma once

#include <stdint.h>

#define MIN_TEMPERATURE				(-20 * 10)
#define MAX_TEMPERATUE				(60  * 10)

#define MAX_HUMIDITY				(100 * 10) // 100%

#define TEMP_HUM_ARRAY_SIZE			(10)


/**
 * Calculates the weighted average from the given array of measurements, 
 * based on the formula avg*0.75 + sample*0.25
 */
int16_t calculateWeightedAverage(int16_t arr[], uint8_t size);

/**
 * Get the weighted average of the temperature as x10
 * e.g 25.3 -> 253
 */
int16_t tempHum_getTemperature(void);

/**
 * Get the weighted average of the humidity as x10.
 * e.g 75.3% -> 753
 */
uint16_t tempHum_getHumidity(void);

/**
 * Create the TempHum task and init the hih8120 driver
 */
void tempHum_task_create(void);

/**
 * Create the TempHum task and init the hih8120 driver
 */
void tempHum_driver_destroy(void);

// for testing
void tempHum_initDriver();
void tempHum_task_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index);

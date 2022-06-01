/*
 * temperature.h
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc & alex
 */ 

#pragma once

#include <stdint.h>
#include <stdbool.h>

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
 * Check the status of the sensor's temperature and humidity measuring modules
 */
bool tempHum_getStatusTemperature();
bool tempHum_getStatusHumidity();

// for testing
void tempHum_initDriver();
void tempHum_task_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index);

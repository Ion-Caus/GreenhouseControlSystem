/*
 * thresholdConfiguration.h
 *
 * Created: 18/05/2022 20.01.37
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>
#include "sensorConfig.h"

#define TEMP_MIN_THRESHOLD			(MIN_TEMPERATURE)
#define TEMP_MAX_THRESHOLD			(MAX_TEMPERATUE)
#define CO2_MAX_THRESHOLD			(MAX_CO2)

/*
 * Create the mutex for the Thresholds
 */
void thresholdMutex_create();

int16_t thresholdMutex_getTemperatureUpper();
int16_t thresholdMutex_getTemperatureLower();
uint16_t thresholdMutex_getCo2Upper();
uint16_t thresholdMutex_getCo2Lower();

void thresholdMutex_setTempUpper(int16_t tempThreshold);
void thresholdMutex_setTempLower(int16_t tempThreshold);
void thresholdMutex_setCo2Upper(uint16_t co2Threshold);
void thresholdMutex_setCo2Lower(uint16_t co2Threshold);
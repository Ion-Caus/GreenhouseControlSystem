/*
 * ThresholdConfiguration.h
 *
 * Created: 18/05/2022 20.01.37
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>

void thresholdMutex_create();
int16_t getTemperatureThresholdUpper();
int16_t getTemperatureThresholdLower();
uint16_t getCo2ThresholdUpper();
uint16_t getCo2ThresholdLower();
void setTempThresholdUpper(int16_t tempThreshold);
void setTempThresholdLower(int16_t tempThreshold);
void setCo2ThresholdUpper(uint16_t co2Threshold);
void setCo2ThresholdLower(uint16_t co2Threshold);
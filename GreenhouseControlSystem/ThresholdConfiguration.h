/*
 * ThresholdConfiguration.h
 *
 * Created: 18/05/2022 20.01.37
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>

void create();
int16_t getTemperatureThreshold();
uint16_t getHumidityThreshold();
uint16_t getCo2Threshold();
void setTempThreshold(int16_t tempThreshold);
void setHumThreshold(uint16_t humThreshold);
void setCo2Threshold(uint16_t co2Threshold);
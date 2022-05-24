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

SemaphoreHandle_t avg_calc_mutex;

int16_t calculateWeightedAverage(int16_t array[], uint8_t size);

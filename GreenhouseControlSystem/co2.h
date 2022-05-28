/*
 * co2.h
 *
 * Created: 18/05/2022 19.12.12
 *  Author: alex
 */ 

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_CO2					( 5000 ) // max ppm

#define CO2_ARRAY_SIZE			( 10 )

uint16_t co2_getCo2();

void co2_initDriver();
void co2_createTask(void);
bool co2_getStatus();


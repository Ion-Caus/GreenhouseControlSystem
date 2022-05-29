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

/**
 * Get the weighted average of the co2 ppm
 */
uint16_t co2_getCo2();

/**
 * Create the co2 task and init the mh_z19 driver
 */
void co2_task_create(void);

/**
 * Check the status of the co2 sensor
 */
bool co2_getStatus();

//for testing 
void co2_initDriver();
void co2_task_run(int16_t* co2Array, uint8_t* index);

/*
 * co2.h
 *
 * Created: 18/05/2022 19.12.12
 *  Author: alex
 */ 

#pragma once

#include <stdint.h>


#define MAX_CO2					( 5000 ) // max ppm

#define CO2_ARRAY_SIZE			( 10 )


/**
 * Get the weighted average of the co2 in ppm.
 */
uint16_t co2_getCo2(void);

/**
 * Create the Co2 task and init the co2 driver
 */
void co2_task_create(void);


// for testing
void co2_initDriver(void);
void co2_task_run(int16_t* co2Array, uint8_t* index);
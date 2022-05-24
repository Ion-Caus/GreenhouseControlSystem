/*
 * moisture.h
 *
 * Created: 5/18/2022 1:36:07 PM
 *  Author: Lukas
 */ 

#pragma once

#include <stdint.h>

#define POT_COUNT					(6)
#define POT_DELAY_MS				(80)

											// lower number means the change is faster and vice versa
#define CHANGE_SPEED_INVERSE		(40)	// number higher then 200 will make the temperatures not change at all

#define TOP_MOISTURE				(70 - 1) // the minus one is for buffer
#define LOW_MOISTURE				(40 + 1) // the plus one is for buffer


/**
 * Get an array of size 6, containing moisture samples.
 * The index of the value in the array is the id of the pod.
 */
uint8_t* moisture_getMoistures(void);

/**
 * Create the TempHum task and init the moisture driver
 */
void moisture_task_create(void);


// for testing
void moisture_initDriver(void);
void moisture_taskRun(void);

/*
 * Generates a fake measurement based on the previous one.
 */
uint8_t _fake_moisture_measurement(uint8_t previousMeasurement);
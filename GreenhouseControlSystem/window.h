/*
 * window.h
 *
 * Created: 22/05/2022 13.37.20
 *  Author: alex
 */ 
#pragma once

#include "sensorDataPackageHandler.h"

#define SERVO_NO  ( 0 ) // servo J14

#define WINDOW_CLOSE_MARGIN			(0.75)
#define WINDOW_HALF_MARGIN			(0.9)

/**
 * Create the Window task and init the servo driver
 */
void window_task_create(void);


// for testing
void window_initDriver();
void window_task_run(measurements_t receivedData, int8_t* percent);

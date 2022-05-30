/*
 * window.c
 *
 * Created: 22/05/2022 13.37.38
 *  Author: alex
 */ 

#include <ATMEGA_FreeRTOS.h>
#include <rc_servo.h>
#include <stdio.h>

#include "window.h"
#include "sensorDataPackageHandler.h"
#include "thresholdConfiguration.h"
#include "buffersHandler.h"
#include "config.h"
#include "sensorConfig.h"


extern MessageBufferHandle_t windowBuffer;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = pdMS_TO_TICKS(WINDOW_DELAY_MS);

void window_task_run(measurements_t receivedData, int8_t* percent) 
{

	// receiving the package from the window buffer
	xMessageBufferReceive(windowBuffer,
	(void*)&receivedData,
	sizeof(measurements_t),
	portMAX_DELAY);
	
 	printf("Received message from window buffer\n");
	
	int16_t	minTemp = thresholdMutex_getTemperatureLower();
	int16_t	maxTemp = thresholdMutex_getTemperatureUpper();
	uint16_t maxCo2 = thresholdMutex_getCo2Upper();
	
	// before threshold values are received, numbers are just maximum of our greenhouse, from -20 to 60 degrees
	// servo is going to act only on temperature and co2 levels, where temperature is of a higher priority
		
	if (receivedData.temperature < (minTemp + ((maxTemp-minTemp) * WINDOW_CLOSE_MARGIN))) {
		*percent = 0; // close window fully - too cold
	}
	else if (receivedData.temperature >= maxTemp * WINDOW_HALF_MARGIN && receivedData.temperature <= maxTemp) {
		*percent = 50; // open window halfway
	}
	else if (receivedData.temperature > maxTemp || receivedData.co2 > maxCo2) {
		*percent = 100; // open window fully - too hot or norm temperature, but co2 too high
	}

	rc_servo_setPosition(SERVO_NO, *percent);
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency);
}
 

void window_task(void* pvParameter) {
	(void) pvParameter;
	
	int8_t percent = 0; // starting position, to open fully to the right + 100
	
	xLastWakeTime = xTaskGetTickCount();

	measurements_t receivedData = { 0 };
		
	rc_servo_setPosition(SERVO_NO, percent); // window closed

	for (;;)
	{
		window_task_run(receivedData, &percent);
	}
}

void window_initDriver() {
	rc_servo_initialise();
	puts("Servo driver initialized.\n");
}


void window_task_create(void) {
	
	window_initDriver();
	
	xTaskCreate(
		window_task,
		"Window Task",
		WINDOW_TASK_STACK,
		NULL,
		WINDOW_TASK_PRIORITY,
		NULL
	);
}
/*
 * window.c
 *
 * Created: 22/05/2022 13.37.38
 *  Author: alex
 */ 

#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <rc_servo.h>
#include <stdio.h>
#include <stdbool.h>

#include "window.h"
#include "sensorDataPackageHandler.h"
#include "thresholdConfiguration.h"
#include "buffersHandler.h"
#include "config.h"


extern MessageBufferHandle_t windowBuffer;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency = pdMS_TO_TICKS(WINDOW_DELAY_MS);

static bool isOpen = false;

void window_task_run(measurements_t receivedData, int8_t* percent) 
{

	// receiving the package from the window buffer
	xMessageBufferReceive(windowBuffer,
	(void*)&receivedData,
	sizeof(measurements_t),
	portMAX_DELAY);
	
	printf("Received message from window buffer\n");
	
	// servo is going to act only on temperature and co2 levels, where temperature is of a higher priority
	if (receivedData.temperature > thresholdMutex_getTemperatureUpper() && !isOpen)
	{
		*percent += 100; // open window - too hot
		isOpen = true;
	}
	
	if (receivedData.temperature < thresholdMutex_getTemperatureLower() && isOpen)
	{
		*percent = 0; // close window - too cold
		isOpen = false;
	}
	
	if (receivedData.co2 > thresholdMutex_getCo2Upper() && receivedData.temperature >= thresholdMutex_getCo2Lower() && !isOpen)
	{
		*percent += 100; // open window - too much co2
		isOpen = true;
	}
	
	if ((receivedData.temperature >= thresholdMutex_getTemperatureLower() && receivedData.temperature <= thresholdMutex_getTemperatureUpper())
			&& (receivedData.co2 >= thresholdMutex_getCo2Lower() && receivedData.co2 <= thresholdMutex_getCo2Upper()) && isOpen)
	{
		*percent = 0; // close window - conditions are good
		isOpen = false;
	}
	
	rc_servo_setPosition(SERVO_NO, *percent);
	
	xTaskDelayUntil( &xLastWakeTime, xFrequency);
}

void window_task(void* pvParameter) {
	(void) pvParameter;
	
	int8_t percent = 0; // starting position, to open fully to the right + 100
	
	xLastWakeTime = xTaskGetTickCount();

	measurements_t receivedData;
		
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
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

#include "sensorDataPackageHandler.h"
#include "ThresholdConfiguration.h"

#define WINDOW_DELAY_MS				( 500 )
#define WINDOW_TASK_STACK			( configMINIMAL_STACK_SIZE )
#define WINDOW_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

extern MessageBufferHandle_t windowBuffer;


void initServoDriver() {
	rc_servo_initialise();
	puts("Servo driver initialized.\n");
}


void windowTask(void* pvParameter) {
	(void) pvParameter;
	
	const uint8_t servoNo = 0; // servo J14
	int8_t percent = 0; // starting position, to open fully to the right + 100
	bool isOpen = false;
	
	rc_servo_setPosition(servoNo, percent); // window closed
	
	TickType_t xLastWakeTime;

	const TickType_t xFrequency = pdMS_TO_TICKS(WINDOW_DELAY_MS);
	xLastWakeTime = xTaskGetTickCount();


	measurements_t receivedData = {0};
		
	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

		// receiving the payload from the window buffer
		xMessageBufferReceive(windowBuffer,
		(void*)receivedData,
		sizeof(receivedData),
		portMAX_DELAY);
		
		printf("Received message from window buffer\n");
		
		// servo is going to act only on temperature and co2 levels, where temperature is of a higher priority
		if (receivedData.temperature > getTemperatureThresholdUpper() && !isOpen)
		{
			percent += 100; // open window - too hot
			isOpen = true;
		}
		
		if (receivedData.temperature < getTemperatureThresholdLower() && isOpen)
		{
			percent = 0; // close window - too cold
			isOpen = false;
		}
		
		if (receivedData.co2 > getCo2ThresholdUpper() && receivedData.temperature >= getTemperatureThresholdLower() && !isOpen)
		{
			percent += 100; // open window - too much co2
			isOpen = true;
		}
		
		if ((receivedData.temperature >= getTemperatureThresholdLower() && receivedData.temperature <= getTemperatureThresholdUpper())
			 && (receivedData.co2 >= getCo2ThresholdLower() && receivedData.co2 <= getCo2ThresholdUpper()) && isOpen) 
		{
			percent = 0; // close window - conditions are good 
			isOpen = false;
		}
		
		rc_servo_setPosition(servoNo, percent);
		
		xTaskDelayUntil( &xLastWakeTime, xFrequency);
	}
}

void createWindowTask(void) {
	
	initServoDriver();
	
	xTaskCreate(
	windowTask,
	"Window Task",
	WINDOW_TASK_STACK,
	NULL,
	WINDOW_TASK_PRIORITY,
	NULL
	);
}
/*
 * temperature.c
 *
 * Created: 4/29/2022 11:26:27 PM
 *  Author: ionc
 */ 
#include "temperature.h"

#include <stdint.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hih8120.h>

#include <stdio_driver.h>


#define TEMP_DELAY_MS				(1000 * 1) // 1 sec

// move to .h
#define TEMPERATURE_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define TEMPERATURE_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )


void initTempDriver() {
	hih8120_driverReturnCode_t returnCode = hih8120_initialise();
	
	if (returnCode != HIH8120_OK) {
		puts("Temperature driver failed to initialize.\n");
		return;
	}
	
	puts("Temperature driver initialized.\n");
}


void temperatureTask(void* pvParameter) {
	 (void) pvParameter;
	 
	 TickType_t xLastWakeTime;
	 const TickType_t xFrequency = TEMP_DELAY_MS/portTICK_PERIOD_MS;

	 xLastWakeTime = xTaskGetTickCount();
	 
	 int16_t temperature = 0;
	 
	 for (;;)
	 { 
		  xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(50) );
		 
		 hih8120_driverReturnCode_t returnCode;
		 if (HIH8120_OK != (returnCode = hih8120_wakeup())) {
			 printf("Temperature Driver failed to wake up, %s\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(100) );
		 
		 if (HIH8120_OK != (returnCode = hih8120_measure()) {
			 printf("Temperature Driver failed to measure, %s\n", returnCode);
			 continue;
		 }
		 
		 xTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(10) );
		 
		 
		 temperature = hih8120_getTemperature();
		 printf("Temperature : %d", temperature);
		
		 xTaskDelayUntil( &xLastWakeTime, xFrequency );
	 }
}

void createTemperatureTask(void) {
	
	initTempDriver();
	
	xTaskCreate(
		temperatureTask,		// function task name
		"Temperature Task",		// task name
		TEMPERATURE_TASK_STACK,
		NULL,
		TEMPERATURE_TASK_PRIORITY,
		NULL
	);
}


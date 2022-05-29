/*
 * moisture.c
 *
 * Created: 5/18/2022 5:18:02 PM
 *  Author: Lukas
 */ 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <sen14262.h>

#include "moisture.h"
#include "application.h"

#include "eventGroupsHandler.h"
#include "config.h"


extern EventGroupHandle_t measureEventGroup;
extern EventGroupHandle_t readingsReadyEventGroup;


static uint8_t potsHumidity[POT_COUNT] = {55, 55, 55 ,55 ,55, 55}; // init values that we want to have


uint8_t* moisture_getMoistures(){
	return potsHumidity;
}

uint8_t _fake_moisture_measurement(uint8_t previousMeasurement){
	
	uint16_t currentMeasurement = sen14262_envelope();
	
	int8_t sign;
	if (previousMeasurement > LOW_MOISTURE && previousMeasurement < TOP_MOISTURE)
	{
		sign = ( currentMeasurement % 3 ) - 1; //if it is even let the sign by -1 if odd let it be +1
	}
	else
	{
		sign = (previousMeasurement < LOW_MOISTURE)
		? +1 //if it is already too low don't make it even lower
		: -1; // if it is too high don't make it even higher
	}
	
	return previousMeasurement + (sign* ((int)(sqrt( currentMeasurement / CHANGE_SPEED_INVERSE ))));
}


void moisture_taskRun() {
	xEventGroupWaitBits(measureEventGroup,
		BIT_TASK_MOIST,
		pdFALSE,	//clear the bit so measurement will happen just after someone request it again
		pdTRUE,
		portMAX_DELAY
	);
	
	uint8_t	tempArr[POT_COUNT];

	for (uint8_t i = 0; i < POT_COUNT; i++) {
		
		//make the task wait some time so there will be differences between different faked measurements
		vTaskDelay(pdMS_TO_TICKS(POT_DELAY_MS));
		
		tempArr[i] =  _fake_moisture_measurement(potsHumidity[i]); //the measurements made quite big difference so I divided it by 8
	}
	
	for (uint8_t i = 0; i < POT_COUNT; i++) {
		potsHumidity[i] = tempArr[i];
	}

	xEventGroupSetBits(readingsReadyEventGroup, BIT_TASK_MOIST);
	
	vTaskDelay(pdMS_TO_TICKS(MOISTURE_DELAY_MS));
}


bool moisture_getStatus(){
	return true;
}

void moisture_initDriver() {
	sen14262_initialise();
	puts("Moisture driver initialized.\n");
}


void moisture_task(void* pvParameters) {
	
	for (;;) {
		moisture_taskRun();
	}
}

void moisture_task_create(void) 
{
	moisture_initDriver();
	
	xTaskCreate(
		moisture_task,
		"Moisture task",
		MOIST_TASK_STACK,
		NULL,
		MOIST_TASK_PRIORITY,
		NULL
	);
}
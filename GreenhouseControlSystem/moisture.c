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
#include "moisture.h"

#include <sen14262.h>

#include "application.h"

#define POT_COUNT					(6)
#define MOISTURE_DELAY_MS			(500)
#define POT_DELAY_MS				(80)

// lower number means the change is faster and vice versa
#define CHANGE_SPEED_INVERSE		(40) // number higher then 200 will make the temperatures not change at all

#define TOP_MOISTURE				(70 - 1) // the minus one is for buffer
#define LOW_MOISTURE				(40 + 1) // the plus one is for buffer

extern EventGroupHandle_t _measureEventGroup;
extern EventGroupHandle_t _readingsReadyEventGroup;

uint8_t potsHumidity[6] = {55,55,55,55,55,55}; // init values that we want to have

void moisture_taskRun();
void moisture_task(void* pvParameters);
uint8_t _fake_moisture_measurement(uint8_t);

void moisture_create(){
	//init the driver
	sen14262_initialise();

	//make a task that will wait for bit, make measurements and then set another bit
	moisture_createTask();
}

uint8_t* moisture_getMoistures(){
	return potsHumidity;
}

void moisture_createTask(void) {
	//moisture_create();
	
	xTaskCreate(
		moisture_task,
		"Moisture task",
		MOIST_TASK_STACK,
		NULL,
		MOIST_TASK_PRIORITY,
		NULL
	);
}

void moisture_taskRun() {
	xEventGroupWaitBits(_measureEventGroup,
		BIT_TASK_MOIST,
		pdFALSE, //clear the bit so measurement will happen just after someone request it again
		pdTRUE,
		portMAX_DELAY
	);
	
	uint8_t tempArr[6];

	for (uint8_t i = 0; i < POT_COUNT; i++) {
		//make the task wait some time so there will be differences between different faked measurements
		vTaskDelay(pdMS_TO_TICKS(POT_DELAY_MS));
		
		tempArr[i] =  _fake_moisture_measurement(potsHumidity[i]); //the measurements made quite big difference so I divided it by 8
		//printf("%d, ",potsHumidity[i]);
	}
	//printf(";\n");
	
	// for some reason I could not use potsHumidity = tempArr so this will make it.
	for (uint8_t i = 0; i < 6; i++) {
		potsHumidity[i] = tempArr[i];
	}

	xEventGroupSetBits(_readingsReadyEventGroup, BIT_TASK_MOIST);

	//printf("my bits: %d\n", xEventGroupGetBits(_readingsReadyEventGroup));
	
	vTaskDelay(pdMS_TO_TICKS(MOISTURE_DELAY_MS));
}

void moisture_task(void* pvParameters) {
	
	for (;;) {
		moisture_taskRun();
	}
}

uint8_t _fake_moisture_measurement(uint8_t previousMeasurement){
	uint16_t currentMeasurement = sen14262_envelope();
	//printf("%d, ",currentMeasurement);
	int8_t sign;
	if (previousMeasurement > LOW_MOISTURE && previousMeasurement < TOP_MOISTURE){
		sign = ((currentMeasurement%3)) - 1; //if it is even let the sign by -1 if odd let it be +1
	} else if (previousMeasurement < LOW_MOISTURE){ //if it is already too low don't make it even lower
		sign = +1;
	} else { // if it is too high don't make it even higher
		sign = -1;
	}
	return previousMeasurement + (sign* ((int)(sqrt( currentMeasurement / CHANGE_SPEED_INVERSE ))));
}



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

#include "application.h"

#define POT_COUNT					(6)
#define MOISTURE_DELAY_MS			(500)
#define POT_DELAY_MS				(80)
#define SUMS_PER_POT				(5)

extern EventGroupHandle_t _measureEventGroup;
extern EventGroupHandle_t _readingsReadyEventGroup;

uint16_t potsHumidity[6];

void moisture_taskRun();
void moisture_task(void* pvParameters);

void moisture_create(){
	sen14262_initialise();
	//nothing to do
	moisture_createTask();
}

uint16_t* moisture_getMoistures(){
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
	printf("inside of run task");
	xEventGroupWaitBits(_measureEventGroup,
		BIT_TASK_MOIST,
		pdFALSE, //clear the bit so measurement will happen just after someone request it again
		pdTRUE,
		portMAX_DELAY
	);
	printf("after waiting for bit");
	
	uint16_t tempMoisture[6];
	for (uint8_t i = 0; i < POT_COUNT; i++) {
		//make the task wait some time so there will be differences between different faked measurements
		uint8_t myMoist = 0;
		while (myMoist == 0){
			for (uint8_t i = 0; i < SUMS_PER_POT; i++) {
				vTaskDelay(pdMS_TO_TICKS(POT_DELAY_MS));
				myMoist += sen14262_envelope();
			}
		}
		potsHumidity[i] = myMoist;
		printf("%d, ",potsHumidity[i]);
	}
	printf(";\n");
	
	*potsHumidity = *tempMoisture;

	xEventGroupSetBits(_readingsReadyEventGroup, BIT_TASK_MOIST);
	
	vTaskDelay(pdMS_TO_TICKS(MOISTURE_DELAY_MS));
}

void moisture_task(void* pvParameters) {
	
	for (;;) {
		moisture_taskRun();
	}
}



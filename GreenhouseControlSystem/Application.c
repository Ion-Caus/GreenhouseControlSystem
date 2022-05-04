/*
 * Application.c
 *
 * Created: 04/05/2022 21.32.57
 *  Author: Deniss
 */ 


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include "event_groups.h"
#include "temperature.h"
#include "sensorDataPackageHandler.h"



// move to .h
#define APPLICATION_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define APPLICATION_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 ) //Make this higher than other tasks

EventGroupHandle_t _measureEventGroup = NULL;
EventGroupHandle_t _readingsReadyEventGroup = NULL;

#define BIT_TASK_TEMPERATURE (1<<0)
#define BIT_TASK_CO2 (1<<1)



void initEventGroups(void){
	_measureEventGroup = xEventGroupCreate();
	_readingsReadyEventGroup = xEventGroupCreate();
	
}

void applicationTask(void* pvParameter){
	(void)pvParameter; //discarding parameters;
	
	//once the task is ready 
	int measuredTemperature = getTemperature();
	
	
}

void createApplicationTask(void){
	
	initEventGroups();
	
	xTaskCreate(
	applicationTask,
	"Application Task",
	APPLICATION_TASK_STACK,
	NULL,
	APPLICATION_TASK_PRIORITY,
	NULL
	);
}
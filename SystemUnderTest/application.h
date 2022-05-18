/*
 * application.h
 *
 * Created: 06/05/2022 17.34.15
 *  Author: Deniss
 */ 


#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"


#define APPLICATION_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define APPLICATION_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 ) //Make this higher than other tasks

#define BIT_TASK_TEMPHUM (1<<0)
#define BIT_TASK_CO2 (1<<1)

void createApplicationTask(void);


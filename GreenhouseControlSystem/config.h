/*
 * config.h
 *
 * Created: 5/24/2022 12:14:50 AM
 *  Author: ionc
 */ 

#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <task.h>

#define MAX_RETRIES					(5)

// === Task delay ===
#define TEMP_HUM_DELAY_MS								(200)

#define APPICATION_TASK_DELAY_MS						(2 * 60000UL)
#define UPLINKHANDLER_LORA_TASK_DELAY_MS				(5 * 60000UL)	// Upload message every 5 minutes (5 * 1min)
#define DOWNLINK_HANDLER_TASK_DELAY_MS					(5 * 60000UL)	


// === Priority ===
#define TEMP_HUM_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define APPLICATION_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 ) 

#define UPLINK_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define DOWNLINK_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define LEDS_STATUS_PRIORITY		( tskIDLE_PRIORITY + 5 )


// === Task stack size ===
#define TEMP_HUM_TASK_STACK							( configMINIMAL_STACK_SIZE )
#define APPLICATION_TASK_STACK						( configMINIMAL_STACK_SIZE + 200 ) 
#define UPLINK_TASK_STACK							( configMINIMAL_STACK_SIZE + 200 )
#define DOWNLINK_TASK_STACK 						( configMINIMAL_STACK_SIZE + 200 )

// === Dev envoriment ===
#define DEV_ENV		( 1 )
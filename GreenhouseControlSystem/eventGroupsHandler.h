/*
 * eventGroupsHandler.h
 *
 * Created: 5/23/2022 11:36:40 PM
 *  Author: ionc
 */ 


#ifndef EVENTGROUPSHANDLER_H_
#define EVENTGROUPSHANDLER_H_

#include "ATMEGA_FreeRTOS.h"
#include "event_groups.h"


#define BIT_TASK_TEMPHUM		(1<<0)
#define BIT_TASK_CO2			(1<<1)
#define BIT_TASK_MOIST			(1<<2)


/**
 * The event group controlled by the application task, 
 * used to sync when sensors task must measure.
 */
EventGroupHandle_t measureEventGroup;

/**
 * The event group used to when sensors tasks have ready data,
 * and application task can retrieve it.
 */
EventGroupHandle_t readingsReadyEventGroup;


/**
 * Create the measureEventGroup and readingReadyEventGroup.
 */
void eventGroupsHandler_create(void);

#endif // EVENTGROUPSHANDLER_H_
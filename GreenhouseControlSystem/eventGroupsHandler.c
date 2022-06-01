/*
 * eventGroupsHandler.c
 *
 * Created: 5/23/2022 11:37:00 PM
 *  Author: ionc, deniss
 */ 

#include "eventGroupsHandler.h"

EventGroupHandle_t measureEventGroup;

EventGroupHandle_t readingsReadyEventGroup;

void eventGroupsHandler_create(void)
{
	measureEventGroup = xEventGroupCreate();
	readingsReadyEventGroup = xEventGroupCreate();
}
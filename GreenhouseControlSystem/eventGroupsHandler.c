/*
 * eventGroupsHandler.c
 *
 * Created: 5/23/2022 11:37:00 PM
 *  Author: ionc
 */ 

#include "eventGroupsHandler.h"

void eventGroupsHandler_create(void)
{
	measureEventGroup = xEventGroupCreate();
	readingsReadyEventGroup = xEventGroupCreate();
}
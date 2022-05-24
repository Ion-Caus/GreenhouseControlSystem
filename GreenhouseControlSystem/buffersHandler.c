/*
 * buffersHandler.c
 *
 * Created: 5/23/2022 11:35:44 PM
 *  Author: ionc
 */ 

#include "ATMEGA_FreeRTOS.h"
#include "buffersHandler.h"

#include "lora_driver.h"
#include "sensorDataPackageHandler.h"


#define SIZE_WINDOW_BUFFER				( sizeof(measurements_t) * 2 )
#define SIZE_UPLINK_BUFFER				( sizeof(measurements_t) * 2 )
#define SIZE_DOWNLINK_BUFFER			( sizeof(lora_driver_payload_t) * 2 )


void buffersHandler_create() 
{
	windowBuffer =  xMessageBufferCreate( SIZE_WINDOW_BUFFER );
	upLinkBuffer =  xMessageBufferCreate( SIZE_UPLINK_BUFFER );
	downLinkBuffer = xMessageBufferCreate( SIZE_DOWNLINK_BUFFER );
}
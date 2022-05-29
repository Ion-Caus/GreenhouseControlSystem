/*
 * UpLinkHandler_LoraWan.c
 *
 * Created: 5/4/2022 9:49:46 PM
 *  Author: ionc
 */ 

#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include <status_leds.h>

#include "sensorDataPackageHandler.h"
#include "upLinkHandler_LoraWAN.h"
#include "buffersHandler.h"

#include "lorawanConfig.h"
#include "config.h"

#define NR_BYTES_TO_SWAP		(6)

extern MessageBufferHandle_t upLinkBuffer;

static lora_driver_payload_t _uplink_payload;

static TickType_t xLastWakeTime;
static const TickType_t xFrequency  = pdMS_TO_TICKS(UPLINKHANDLER_LORA_TASK_DELAY_MS);


static void _lora_setup(void)
{
	char _out_buf[20];
	lora_driver_returnCode_t rc;
	status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	rc = lora_driver_rn2483FactoryReset();
	#if DEV_ENV
		printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(rc));
	#endif
	
	// Configure to EU868 LoRaWAN standards
	rc = lora_driver_configureToEu868();
	#if DEV_ENV
		printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(rc));
	#endif
	
	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	#if DEV_ENV
		printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);
	#endif

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	rc = lora_driver_setDeviceIdentifier(_out_buf);
	#if DEV_ENV
		printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(rc));
	#endif
	
	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	rc = lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf);
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(rc));

	// Save all the MAC settings in the transceiver
	rc = lora_driver_saveMac();
	printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(rc));

	// Enable Adaptive Data Rate
	rc = lora_driver_setAdaptiveDataRate(LORA_ON);
	#if DEV_ENV
		printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(rc));
	#endif

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	rc = lora_driver_setReceiveDelay(500);
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(rc));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		if ( rc != LORA_ACCEPTED)
		{
			// Make the red led pulse to tell something went wrong
			status_leds_longPuls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(pdMS_TO_TICKS(5000UL));
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LORA_ACCEPTED)
	{
		// Connected to LoRaWAN :-)
		// Make the green led steady
		status_leds_ledOn(led_ST2); // OPTIONAL
	}
	else
	{
		// Something went wrong
		// Turn off the green led
		status_leds_ledOff(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		status_leds_fastBlink(led_ST1); // OPTIONAL

		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

static void resetLoraWAN(void) {
	// Hardware reset of LoRaWAN transceiver
	lora_driver_resetRn2483(1);
	
	vTaskDelay(2);
	
	lora_driver_resetRn2483(0);
	
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	_lora_setup();

	_uplink_payload.len = UPLINK_PAYLOAD_LEN;
	_uplink_payload.portNo = UPLINK_PAYLOAD_PORTNO;
}

void upLinkHandler_task_run(uint8_t* packageBuffer) 
{
	xTaskDelayUntil( &xLastWakeTime, xFrequency );
	
	// receiving the data sensor package from the upLink buffer
	size_t bytesReceived = xMessageBufferReceive(upLinkBuffer,
			(void*)packageBuffer,
			sizeof(measurements_t),
			portMAX_DELAY);
	
	puts("Received message from UpLinkBuffer\n");
	
	#if DEV_ENV
		// TODO : delete in production
		for (uint8_t i = 0; i < bytesReceived; i++) {
			printf("%d, ", packageBuffer[i]);
		}
		printf("\n");
		// --------------------------
	#endif
	
	// coping the package into the payload

	for (uint8_t i = 0; i < bytesReceived; i++) {
		uint8_t index = i;
		if (i < NR_BYTES_TO_SWAP) {
			// swapping little endian into big endian for the first 6 bytes
			index = (i % 2 == 0) ? i + 1 : i - 1;
		}
		
		_uplink_payload.bytes[i] = packageBuffer[index];
	}

	status_leds_shortPuls(led_ST4); 
	
	// uploading the upLink payload to LoraWAN
	lora_driver_returnCode_t uploadStatus = lora_driver_sendUploadMessage(false, &_uplink_payload);
	
	printf("Upload Message >%s<\n",
		lora_driver_mapReturnCodeToText(uploadStatus));
}


/*-----------------------------------------------------------*/
void upLinkHandler_task( void *pvParameters )
{

	resetLoraWAN(); // Hardware reset of LoRaWAN
	 
	xLastWakeTime = xTaskGetTickCount();
	
	uint8_t packageBuffer[sizeof(measurements_t)] = {0};
	
	for(;;)
	{
		upLinkHandler_task_run(packageBuffer);	
	}
}

void upLinkHandler_task_create()
{	
	xTaskCreate(
	upLinkHandler_task
	,  "UpLinkHandler_LoraWAN" 
	,  UPLINK_TASK_STACK  
	,  NULL
	,  UPLINK_TASK_PRIORITY
	,  NULL );
}
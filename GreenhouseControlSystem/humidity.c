#include "humidity.h"

#include <stdio.h>
#include <stdio_driver.h>

#include <ATMEGA_FreeRTOS.h>
#include <hih8120.h>
#include <task.h>

#define HUMIDITY_TASK_STACK		(configMINIMAL_STACK_SIZE)
#define HUMIDITY_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)
#define RETRIES_LEFT 5

void humidity_init(void)
{
	hih8120_driverReturnCode_t retCode = hih8120_initialise();

	for (int i = 0; i < RETRIES_LEFT; i++)
	{
		if (retCode == HIH8120_OK)
		{
			puts("Humidity driver successfully initialized!\n");
			return;
		}

		printf("Humidity driver failed to initialize! Reason: %d\n", retCode);
		printf("%d retries left.\n", i);
	}
}

void humidity_task(void* pvParameters)
{
	// Remove compiler warnings
	(void)pvParameters;

	const TickType_t xFrequency = 5000;

	TickType_t xLastWakeTime = xTaskGetTickCount();

	uint8_t humidity = 0;

	for (;;)
	{
		vTaskDelay(100);
		int retCode = hih8120_wakeup();
		if (retCode != HIH8120_OK)
		{
			printf("Driver wake up error: %d\n", retCode);
			return;
		}

		vTaskDelay(50);
		retCode = hih8120_measure();
		if (retCode != HIH8120_OK)
		{
			printf("Driver measurment error: %d\n", retCode);
			return;
		}

		vTaskDelay(50);
		humidity = hih8120_getHumidityPercent_x10();
		printf("Humidity is at: %d percent.\n", humidity);

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void create_humidity_task(void)
{
	humidity_init();

	xTaskCreate(
		humidity_task,
		"HumidityTask",
		HUMIDITY_TASK_STACK,
		NULL,
		HUMIDITY_TASK_PRIORITY,
		NULL
	);
}
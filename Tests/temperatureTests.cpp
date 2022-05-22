/*
 * temperatureTest.cpp
 *
 * Created: 5/19/2022 12:13:02 PM
 *  Author: Lukas, Ion
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>

extern "C" {
	#include "temperature.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <hih8120.h>
	#include <stdio_driver.h>
}

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_initialise);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);
FAKE_VALUE_FUNC(int16_t, hih8120_getTemperature_x10);

FAKE_VALUE_FUNC(BaseType_t, xTaskDelayUntil, TickType_t*, TickType_t);


class TemperatureTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(hih8120_initialise);
		RESET_FAKE(hih8120_wakeup);
		RESET_FAKE(hih8120_measure);
		RESET_FAKE(hih8120_getTemperature_x10);
		RESET_FAKE(xTaskDelayUntil)
	}
	void TearDown() override {}
};


TEST_F(TemperatureTest, temperature_task_run) {
	hih8120_initialise_fake.return_val = HIH8120_OK;

	//createTemperatureTask();
	ASSERT_EQ(vTaskDelayUntil_fake.call_count, 3);
}

TEST_F(TemperatureTest, temperature_succesfull_init) {
	hih8120_initialise_fake.return_val = HIH8120_OK;

	//createTemperatureTask();
	ASSERT_EQ(hih8120_initialise_fake.call_count, 1);
}
/*
 * window_Tests.cpp
 *
 * Created: 30/05/2022 08.22.13
 *  Author: alex
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>


extern "C" {
	#include "window.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <rc_servo.h>
	#include <stdio_driver.h>

	#include "sensorDataPackageHandler.h"
	#include "thresholdConfiguration.h"
	#include "buffersHandler.h"
	#include "config.h"
	#include "sensorConfig.h"
}


FAKE_VOID_FUNC(rc_servo_initialise);
FAKE_VOID_FUNC(rc_servo_setPosition, uint8_t, int8_t); // first - servoNo, second - percentage to open window
FAKE_VALUE_FUNC(int16_t, thresholdMutex_getTemperatureLower);
FAKE_VALUE_FUNC(int16_t, thresholdMutex_getTemperatureUpper);
FAKE_VALUE_FUNC(uint16_t, thresholdMutex_getCo2Upper);


class WindowTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(rc_servo_initialise);
		RESET_FAKE(rc_servo_setPosition);
		RESET_FAKE(thresholdMutex_getTemperatureLower);
		RESET_FAKE(thresholdMutex_getTemperatureUpper);
		RESET_FAKE(thresholdMutex_getCo2Upper);
		RESET_FAKE(xMessageBufferReceive);
	}
	void TearDown() override {}
};

TEST_F(WindowTest, init_servo_driver) {
	window_initDriver();
	ASSERT_EQ(rc_servo_initialise_fake.call_count, 1);
}

TEST_F(WindowTest, window_setPosition) {

	uint8_t servoNo = 0;
	int8_t percent = 50;
	rc_servo_setPosition(servoNo, percent);

	ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
	ASSERT_EQ(rc_servo_setPosition_fake.arg0_val, 0);
	ASSERT_EQ(rc_servo_setPosition_fake.arg1_val, 50);
}


TEST_F(WindowTest, run_window_task) {

	uint8_t moist[6] = {55, 55, 55, 55, 55, 55};

	measurements_t receivedData;
	receivedData.temperature = 231;
	receivedData.humidity = 705;
	receivedData.co2 = 998;
	receivedData.moistures[0] = *moist;
	receivedData.status = 15;

	int8_t value = 0;
	int8_t *percent = &value;

	window_task_run(receivedData, percent);


	ASSERT_EQ(thresholdMutex_getTemperatureLower_fake.call_count, 1);
	ASSERT_EQ(thresholdMutex_getTemperatureLower_fake.return_val, 0); // threshold is not set, so it's 0

	ASSERT_EQ(thresholdMutex_getTemperatureUpper_fake.call_count, 1);
	ASSERT_EQ(thresholdMutex_getTemperatureUpper_fake.return_val, 0); // threshold is not set, so it's 0

	ASSERT_EQ(thresholdMutex_getCo2Upper_fake.call_count, 1);
	ASSERT_EQ(thresholdMutex_getCo2Upper_fake.return_val, 0); // threshold is not set, so it's 0

	ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
	ASSERT_EQ(rc_servo_setPosition_fake.arg0_val, 0);
	ASSERT_EQ(rc_servo_setPosition_fake.arg1_val, 100); // because threshold is currently at zero - window should fully open since received temp is higher
}
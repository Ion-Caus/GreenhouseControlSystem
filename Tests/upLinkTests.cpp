/*
 * upLinkTests.cpp
 *
 * Created: 5/29/2022 10:13:23 AM
 * Author: ionc
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>

extern "C" {
	#include "upLinkHandler_LoraWAN.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <hih8120.h>
	#include <stdio_driver.h>
	#include <lora_driver.h>
	#include <status_leds.h>

	#include "sensorDataPackageHandler.h"
	#include "thresholdConfiguration.h"
}


FAKE_VOID_FUNC(status_leds_slowBlink, status_leds_t);
FAKE_VALUE_FUNC(char*, lora_driver_mapReturnCodeToText, lora_driver_returnCode_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_rn2483FactoryReset);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_configureToEu868);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_getRn2483Hweui, char*);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setDeviceIdentifier, const char*);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setOtaaIdentity, char*, char*, char*);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_saveMac);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setAdaptiveDataRate, lora_driver_adaptiveDataRate_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setReceiveDelay, uint16_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_join, lora_driver_joinMode_t);
FAKE_VOID_FUNC(status_leds_longPuls, status_leds_t);
FAKE_VOID_FUNC(status_leds_ledOff, status_leds_t);
FAKE_VOID_FUNC(status_leds_ledOn, status_leds_t);
FAKE_VOID_FUNC(status_leds_fastBlink, status_leds_t);
FAKE_VOID_FUNC(status_leds_shortPuls, status_leds_t);

FAKE_VOID_FUNC(lora_driver_resetRn2483, uint8_t);
FAKE_VOID_FUNC(lora_driver_flushBuffers);

FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_sendUploadMessage, bool, lora_driver_payload_t*);

FAKE_VALUE_FUNC(int, taskYIELD);

class UplinkTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(status_leds_slowBlink);
		RESET_FAKE(lora_driver_mapReturnCodeToText);
		RESET_FAKE(lora_driver_rn2483FactoryReset);
		RESET_FAKE(lora_driver_configureToEu868);
		RESET_FAKE(lora_driver_getRn2483Hweui);
		RESET_FAKE(lora_driver_setDeviceIdentifier);
		RESET_FAKE(lora_driver_setOtaaIdentity);
		RESET_FAKE(lora_driver_saveMac);
		RESET_FAKE(lora_driver_setAdaptiveDataRate);
		RESET_FAKE(lora_driver_setReceiveDelay);
		RESET_FAKE(lora_driver_join);
		RESET_FAKE(status_leds_longPuls);
		RESET_FAKE(status_leds_ledOff);
		RESET_FAKE(status_leds_ledOn);
		RESET_FAKE(status_leds_fastBlink);
		RESET_FAKE(status_leds_shortPuls);
		RESET_FAKE(lora_driver_resetRn2483);
		RESET_FAKE(lora_driver_flushBuffers);
		RESET_FAKE(lora_driver_sendUploadMessage);
		RESET_FAKE(vTaskDelay);

		RESET_FAKE(xMessageBufferReceive);
	}
	void TearDown() override {}
};



// fake recieve function for message buffer for the test - uplink
static size_t uplinkMessageBufferReceive(MessageBufferHandle_t arg0, void* arg1, size_t arg2, TickType_t arg3)
{
	const uint8_t len = 13;
						// 245		755		1100	
	uint8_t bytes[len] = { 245, 0, 243, 2, 76, 4, 56, 57, 55, 56, 57, 59, 15 }; // small endian

	void* pos = arg1;


	for (size_t i = 0; i < len; i++)
	{
		((uint8_t*)arg1)[i] = bytes[i];
	}

	arg1 = pos;
	return len + 2;
}


TEST_F(UplinkTest, uplinkHandler_run_swap_endian_test) {
	// Arrange
	// check fake function - messageBufferReceive
	xMessageBufferReceive_fake.custom_fake = uplinkMessageBufferReceive;
	uint8_t packageBuffer[sizeof(measurements_t)] = { 0 };

	// Act
	upLinkHandler_task_run(packageBuffer);

	// Assert

}


TEST_F(UplinkTest, uplinkHanlder_task_run_test) {
	//Arrange
	uint8_t packageBuffer[sizeof(measurements_t)] = { 0 };

	// Act
	upLinkHandler_task_run(packageBuffer);

	// Assert
	ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
	ASSERT_EQ(lora_driver_sendUploadMessage_fake.call_count, 1);
}

/*
 * tempHum_Test.cpp
 *
 * Created: 5/22/2022 10:13:23 AM
 * Author: ionc
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>

extern "C" {
	#include "downLinkHandler.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <hih8120.h>
	#include <stdio_driver.h>
	#include <lora_driver.h>

	#include "thresholdConfiguration.h"
}


FAKE_VOID_FUNC(thresholdMutex_setTempLower, int16_t);
FAKE_VOID_FUNC(thresholdMutex_setTempUpper, int16_t);
FAKE_VOID_FUNC(thresholdMutex_setCo2Lower, uint16_t);
FAKE_VOID_FUNC(thresholdMutex_setCo2Upper, uint16_t);


class DownlinkTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(thresholdMutex_setTempLower);
		RESET_FAKE(thresholdMutex_setTempUpper);
		RESET_FAKE(thresholdMutex_setCo2Lower);
		RESET_FAKE(thresholdMutex_setCo2Upper);

		RESET_FAKE(xMessageBufferReceive);
	}
	void TearDown() override {}
};



// fake recieve function for message buffer for the test - downlink_bytes_to_int
size_t messageBufferReceive(MessageBufferHandle_t arg0, void* arg1, size_t arg2, TickType_t arg3)
{
	const uint8_t len = 8;
						// 150		295		200		1400
	uint8_t bytes[len] = { 0, 150, 1, 39, 0, 200, 5, 120 }; // big endian

	void* pos = arg1;

	((uint8_t*)arg1)[0] = 1; // portNo
	((uint8_t*)arg1)[1] = 8; // len

	for (size_t i = 2; i < len + 2; i++)
	{
		((uint8_t*)arg1)[i] = bytes[i - 2];
	}

	arg1 = pos;
	return len+2;
}


TEST_F(DownlinkTest, downlink_bytes_to_int) {
	// Arrange
	// check fake function - messageBufferReceive
	xMessageBufferReceive_fake.custom_fake = messageBufferReceive;

	// Act
	downLinkHandler_task_run();

	// Assert
	ASSERT_EQ(thresholdMutex_setTempLower_fake.arg0_val, 150); // 15.0 C 
	ASSERT_EQ(thresholdMutex_setTempUpper_fake.arg0_val, 295); // 29.5 C
	ASSERT_EQ(thresholdMutex_setCo2Lower_fake.arg0_val, 200); // ppm
	ASSERT_EQ(thresholdMutex_setCo2Upper_fake.arg0_val, 1400); // ppm
}


TEST_F(DownlinkTest, downlink_task_run_Zero_bytes_received) {
	//Arrange
	

	// Act
	downLinkHandler_task_run();

	// Assert
	ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);

	ASSERT_EQ(thresholdMutex_setTempLower_fake.call_count, 0);
	ASSERT_EQ(thresholdMutex_setTempUpper_fake.call_count, 0);
	ASSERT_EQ(thresholdMutex_setCo2Lower_fake.call_count, 0);
	ASSERT_EQ(thresholdMutex_setCo2Upper_fake.call_count, 0);
}
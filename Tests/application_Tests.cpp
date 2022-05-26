#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>


extern "C" 
{
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include "application.h"
#include "moisture.h"
#include "co2.h"
#include "temp_hum.h"
#include "sensorDataPackageHandler.h"
}

FAKE_VALUE_FUNC(int16_t, tempHum_getTemperature);

class ApplicationTest : public ::testing::Test {
protected:
	void SetUp() override 
	{
		RESET_FAKE(xEventGroupWaitBits);
		RESET_FAKE(xEventGroupSetBits);
		RESET_FAKE(xEventGroupClearBits);
		RESET_FAKE(xMessageBufferIsEmpty);
		RESET_FAKE(xMessageBufferReset);
		RESET_FAKE(xMessageBufferSend);
	}
	void TearDown() override {}
};


TEST_F(ApplicationTest, application_test_eventgroups) {

	// make the task run
	application_task_run();

	//check if the task sets event group bits corectly

	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 3);
	ASSERT_EQ(xEventGroupSetBits_fake.arg1_history[0], 1 << 0);
	ASSERT_EQ(xEventGroupSetBits_fake.arg1_history[1], 1 << 1);
	ASSERT_EQ(xEventGroupSetBits_fake.arg1_history[2], 1 << 2);

	//check that the task waits for the measurements to be ready
	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1);

	//check that the task clears the bits after receiving the measurements
	ASSERT_EQ(xEventGroupClearBits_fake.call_count, 1);

	ASSERT_EQ(xEventGroupClearBits_fake.arg1_val, 1 << 0 | 1 << 1 | 1 << 2);

}

TEST_F(ApplicationTest, application_test_measurements) {

	// make the task run
	application_task_run();

	//cannot test without mocks
}

TEST_F(ApplicationTest, application_test_dataPackage) {

	// make the task run
	application_task_run();

	ASSERT_EQ(sizeof(sensorDataPackage_getSensorData()), 14);


}

TEST_F(ApplicationTest, application_test_messagebufferIsEmpty) {

	xMessageBufferIsEmpty_fake.return_val = 1;

	// make the task run
	application_task_run();

	ASSERT_EQ(xMessageBufferIsEmpty_fake.call_count, 1);

	ASSERT_EQ(xMessageBufferReset_fake.call_count, 0);
}


TEST_F(ApplicationTest, application_test_messagebufferIsNotEmpty) {

	xMessageBufferIsEmpty_fake.return_val = 0;

	// make the task run
	application_task_run();

	ASSERT_EQ(xMessageBufferIsEmpty_fake.call_count, 1);
	ASSERT_EQ(xMessageBufferReset_fake.call_count, 1);
}

TEST_F(ApplicationTest, application_test_messagebufferSend) {


	// make the task run
	application_task_run();

	ASSERT_EQ(xMessageBufferSend_fake.call_count, 2);
}








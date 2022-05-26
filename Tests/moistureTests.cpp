	/*
 * moistureTests.cpp
 *
 * Created: 5/18/2022 6:13:02 PM
 *  Author: Lukas
 */
#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>

extern "C" {
	#include "moisture.h"
	#include <ATMEGA_FreeRTOS.h>
	#include <task.h>
	#include <semphr.h>
	#include <sen14262.h>
	#include <stdio_driver.h>
	#include "eventGroupsHandler.h"
}

void sen14262_initialise(void);

FAKE_VOID_FUNC(sen14262_initialise);
FAKE_VALUE_FUNC(uint16_t, sen14262_envelope);


class MoistureTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(sen14262_initialise);
		RESET_FAKE(sen14262_envelope);
		RESET_FAKE(xEventGroupWaitBits);
		RESET_FAKE(xEventGroupSetBits);

	}
	void TearDown() override {}
};


TEST_F(MoistureTest, init_moisture_driver) {
	moisture_initDriver();
	ASSERT_EQ(sen14262_initialise_fake.call_count, 1);
}

TEST_F(MoistureTest, get_defalut_array_of_moisture) {
	uint8_t* moistures = moisture_getMoistures();

	ASSERT_EQ(moistures[0], 55);
	ASSERT_EQ(moistures[1], 55);
	ASSERT_EQ(moistures[2], 55);
	ASSERT_EQ(moistures[3], 55);
	ASSERT_EQ(moistures[4], 55);
	ASSERT_EQ(moistures[5], 55);
}

TEST_F(MoistureTest, get_moisture_call) {
	moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 0);
}

TEST_F(MoistureTest, get_moisture_task) {

	// make the task run
	moisture_taskRun();

	//check if the task waits
	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1);
	ASSERT_EQ(xEventGroupWaitBits_fake.arg1_val, BIT_TASK_MOIST);

	//check if the task measure
	ASSERT_EQ(sen14262_envelope_fake.call_count, 6);


	//check if the task give info about finishing
	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 1);
	ASSERT_EQ(xEventGroupSetBits_fake.arg1_val, BIT_TASK_MOIST);
}

TEST_F(MoistureTest, get_moisture_task_value) {
	uint8_t myFakeValue = 2;

	sen14262_envelope_fake.return_val = myFakeValue;

	// make the task run
	moisture_taskRun();

	uint8_t *myPots;

	myPots = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 6	);

	ASSERT_LT(myPots[0], TOP_MOISTURE + 2);
	ASSERT_GT(myPots[0], LOW_MOISTURE - 2);
}

TEST_F(MoistureTest, get_moisture_task_value_all) {
	uint8_t myFakeValue = 2;

	sen14262_envelope_fake.return_val = myFakeValue;

	// make the task run
	moisture_taskRun();

	uint8_t* myPots;

	myPots = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 6);

	for (int i = 0; i < 6; i++) {
		ASSERT_LT(myPots[i], TOP_MOISTURE + 2);
		ASSERT_GT(myPots[i], LOW_MOISTURE - 2);
	}
	
}

TEST_F(MoistureTest, get_moisture_fake_generator_many) {
	uint8_t tempNum;

	sen14262_envelope_fake.return_val = 5;
	tempNum = _fake_moisture_measurement(55);
	ASSERT_LT(tempNum, TOP_MOISTURE + 2);
	ASSERT_GT(tempNum, LOW_MOISTURE - 2);

	sen14262_envelope_fake.return_val = (CHANGE_SPEED_INVERSE *3) + 1;
	tempNum = _fake_moisture_measurement(60);
	ASSERT_LT(tempNum, TOP_MOISTURE + 2);
	ASSERT_GT(tempNum, LOW_MOISTURE - 2);

	sen14262_envelope_fake.return_val = (CHANGE_SPEED_INVERSE * 3) + 2;
	tempNum = _fake_moisture_measurement(50);
	ASSERT_LT(tempNum, TOP_MOISTURE + 2);
	ASSERT_GT(tempNum, LOW_MOISTURE - 2);
}

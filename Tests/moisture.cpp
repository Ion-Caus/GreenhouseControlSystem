/*
 * moisture.cpp
 *
 * Created: 5/18/2022 6:13:02 PM
 *  Author: Lukas
 */
#pragma once

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>
//DEFINE_FFF_GLOBALS

extern "C" {
	#include "moisture.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <moisture.c>
	#include <sen14262.h>

	#include <stdio_driver.h>
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
	}
	void TearDown() override {}
};

TEST_F(MoistureTest, init_moisture) {
	moisture_create();
}

TEST_F(MoistureTest, init_call_moisture) {
	moisture_create();
	ASSERT_EQ(sen14262_initialise_fake.call_count, 1);
}

TEST_F(MoistureTest, get_some_moisture) {
	uint8_t* moistures = moisture_getMoistures();
}

TEST_F(MoistureTest, get_moisture_call) {
	moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 0);
}

TEST_F(MoistureTest, get_moisture_task) {
	// set the bit so measurement will take place
	xEventGroupSetBits(_measureEventGroup, BIT_TASK_MOIST);

	// make the task run
	moisture_taskRun();

	//check if the task waits
	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1);
	ASSERT_EQ(xEventGroupWaitBits_fake.arg1_val, BIT_TASK_MOIST);

	//check if the task measure
	ASSERT_EQ(sen14262_envelope_fake.call_count, 6);


	//check if the task give info about finishing
	//expect this to be 2 because I am calling once and method calls it once
	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 2);
	ASSERT_EQ(xEventGroupSetBits_fake.arg1_val, BIT_TASK_MOIST);
}

TEST_F(MoistureTest, get_moisture_task_value) {
	uint8_t myFakeValue = 2;
	xEventGroupSetBits(_measureEventGroup, BIT_TASK_MOIST);

	sen14262_envelope_fake.return_val = myFakeValue;

	// make the task run
	moisture_taskRun();

	uint8_t *myPots;

	myPots = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, (uint8_t) -1);

	ASSERT_LT(myPots[0], TOP_MOISTURE + 2);
	ASSERT_GT(myPots[0], LOW_MOISTURE - 2);
}

TEST_F(MoistureTest, get_moisture_task_value_all) {
	uint8_t myFakeValue = 2;
	xEventGroupSetBits(_measureEventGroup, BIT_TASK_MOIST);

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

/*
 * moisture.cpp
 *
 * Created: 5/18/2022 6:13:02 PM
 *  Author: Lukas
 */
#pragma once

#include "gtest/gtest.h"
#include "../../fff/fff.h"
DEFINE_FFF_GLOBALS

extern "C" {
	#include <moisture.h>
	#include <moisture.c>
	#include <sen14262.h>
}
void sen14262_initialise();

uint16_t sen14262_envelope();

FAKE_VOID_FUNC(sen14262_initialise);

FAKE_VALUE_FUNC(uint16_t, sen14262_envelope);


class MoistureTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(sen14262_initialise);
		RESET_FAKE(sen14262_envelope);
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
	uint16_t* moistures = moisture_getMoistures();
}

TEST_F(MoistureTest, get_moisture_call) {
	moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 1);
}

TEST_F(MoistureTest, get_moisture_data) {
	uint16_t* moistureReadings = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 1);
	ASSERT_NE(moistureReadings, (uint16_t*) NULL);
}

TEST_F(MoistureTest, get_moisture_real_data) {
	uint16_t myFakeReading = 200;
	sen14262_envelope_fake.return_val = myFakeReading;
	uint16_t* moistureReadings = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 1);
	ASSERT_EQ(moistureReadings[0], myFakeReading);
}

TEST_F(MoistureTest, get_moisture_real_data_all) {
	uint16_t myFakeReading = 200;
	sen14262_envelope_fake.return_val = myFakeReading;
	uint16_t* moistureReadings = moisture_getMoistures();

	ASSERT_EQ(sen14262_envelope_fake.call_count, 1);
	for (uint8_t i = 0; i < 6; i++) {
		ASSERT_EQ(moistureReadings[i], myFakeReading);
	}
}

//testing with freeRTOS with checking if the measurement happen just after bit is sent


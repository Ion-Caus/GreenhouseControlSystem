/*
 * co2_Tests.cpp
 *
 * Created: 6/22/2022 11:26:40 AM
 * Author: Deniss
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>


extern "C"
{
#include "co2.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include <mh_z19.h>
}

FAKE_VOID_FUNC(mh_z19_initialise, serial_comPort_t);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);



class Co2Test : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(mh_z19_initialise);
		RESET_FAKE(mh_z19_takeMeassuring);
		RESET_FAKE(mh_z19_getCo2Ppm);
		RESET_FAKE(xEventGroupWaitBits);
		RESET_FAKE(xEventGroupSetBits);
	}
	void TearDown() override {}
};

static mh_z19_returnCode_t mh_z19_getCo2PpmFake(uint16_t* ppm) {

	*ppm = 1000;

	return MHZ19_OK;
}

TEST_F(Co2Test, init_co2_driver) {
	co2_initDriver();
	ASSERT_EQ(mh_z19_initialise_fake.call_count, 1);
	ASSERT_EQ(mh_z19_initialise_fake.arg0_val, ser_USART3);
}

TEST_F(Co2Test, co2_task_run_getLastMeasurement) {

	//ARRANGE
	mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;
	mh_z19_getCo2Ppm_fake.custom_fake = mh_z19_getCo2PpmFake;
	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,1800,1700,1900,1400, 0 };
	uint8_t index = 9;

	//ACT
	co2_task_run(co2ArraySamples, &index);

	//ASSERT
	ASSERT_EQ(co2_getCo2(), 1420);
	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1);
	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 1);
	ASSERT_EQ(co2ArraySamples[9], 1000);
}

TEST_F(Co2Test, co2_task_run_lastMeasurementFailed) {

	mh_z19_takeMeassuring_fake.return_val = MHZ19_NO_MEASSURING_AVAILABLE;

	mh_z19_getCo2Ppm_fake.custom_fake = mh_z19_getCo2PpmFake;


	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,1800,1700,1900,1400, 0 };
	uint8_t index = 9;
	co2_task_run(co2ArraySamples, &index);

	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 0);
	ASSERT_EQ(co2ArraySamples[9], 0);

}

TEST_F(Co2Test, co2_task_run_midway) {

	mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;

	mh_z19_getCo2Ppm_fake.custom_fake = mh_z19_getCo2PpmFake;


	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,0,0,0,0,0 };
	uint8_t index = 5;
	co2_task_run(co2ArraySamples, &index);


	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1);
	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 0);

}

TEST_F(Co2Test, co2_status_OK) {
	mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;

	mh_z19_getCo2Ppm_fake.return_val = MHZ19_OK;

	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,0,0,0,0,0 };
	uint8_t index = 5;
	co2_task_run(co2ArraySamples, &index);

	ASSERT_EQ(co2_getStatus(), true);
}

TEST_F(Co2Test, co2_status_noMeasuring) {
	mh_z19_takeMeassuring_fake.return_val = MHZ19_NO_MEASSURING_AVAILABLE;

	mh_z19_getCo2Ppm_fake.return_val = MHZ19_OK;

	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,0,0,0,0,0 };
	uint8_t index = 5;
	co2_task_run(co2ArraySamples, &index);

	ASSERT_EQ(co2_getStatus(), false);
}

TEST_F(Co2Test, co2_status_cantGetCo2) {
	mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;

	mh_z19_getCo2Ppm_fake.return_val = MHZ19_NO_MEASSURING_AVAILABLE;

	int16_t co2ArraySamples[CO2_ARRAY_SIZE] = { 1000,2000,1100,1900,1200,0,0,0,0,0 };
	uint8_t index = 5;
	co2_task_run(co2ArraySamples, &index);

	ASSERT_EQ(co2_getStatus(), false);
}
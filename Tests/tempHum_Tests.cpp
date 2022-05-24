/*
 * tempHum_Test.cpp
 *
 * Created: 5/22/2022 10:13:23 AM
 * Author: ionc
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>

extern "C" {
	#include "temp_hum.h"
	#include <FreeRTOS_FFF_MocksDeclaration.h>
	#include <task.h>
	#include <semphr.h>
	#include <hih8120.h>
	#include <stdio_driver.h>
	#include "weighted_average.h"
}

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_initialise);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);
FAKE_VALUE_FUNC(int16_t, hih8120_getTemperature_x10);
FAKE_VALUE_FUNC(uint16_t, hih8120_getHumidityPercent_x10);

FAKE_VALUE_FUNC(BaseType_t, xTaskDelayUntil, TickType_t*, TickType_t);


class TempHumTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(hih8120_initialise);
		RESET_FAKE(hih8120_wakeup);
		RESET_FAKE(hih8120_measure);
		RESET_FAKE(hih8120_getTemperature_x10);
		RESET_FAKE(hih8120_getHumidityPercent_x10);
		RESET_FAKE(xEventGroupWaitBits);
		RESET_FAKE(xEventGroupSetBits);
		RESET_FAKE(xTaskDelayUntil);
	}
	void TearDown() override {}
};


TEST_F(TempHumTest, calculte_weighted_average_tempHum) {
	// Arrange
	int16_t samples[10] = { 230, 234, 232, 234, 290, 230, 232, 234, 234, 231 };

	// Act
	int16_t weightedAverage = calculateWeightedAverage(samples, 10);

	// Assert
	ASSERT_EQ(weightedAverage, 237); // 23.7 
}



TEST_F(TempHumTest, temHum_task_run) {
	//Arrange
	hih8120_wakeup_fake.return_val = HIH8120_OK;
	hih8120_measure_fake.return_val = HIH8120_OK;

	hih8120_getTemperature_x10_fake.return_val = 231;
	hih8120_getHumidityPercent_x10_fake.return_val = 761;

	int16_t tempArr[10] = { 230, 234, 232, 234, 290, 230, 232, 234, 234, 0 };
	int16_t humArr[10] = { 760, 764, 762, 764, 805, 760, 762, 764, 764, 0 };

	uint8_t index = 9;

	// Act
	// missing the last sample, then calling the weigthed average
	tempHum_task_run(tempArr, humArr, &index);

	// Assert
	ASSERT_EQ(xEventGroupWaitBits_fake.call_count, 1); 

	ASSERT_EQ(xTaskDelayUntil_fake.call_count, 4);

	ASSERT_EQ(xEventGroupSetBits_fake.call_count, 1); // set the bit that the weighted averages are ready

	ASSERT_EQ(tempHum_getTemperature(), 237); // weighted average for temerature;
	ASSERT_EQ(tempHum_getHumidity(), 766); // weighted average for humidity;

	ASSERT_EQ(index, 0); // reset index
}

TEST_F(TempHumTest, temperature_succesfull_init) {
	hih8120_initialise_fake.return_val = HIH8120_OK;

	tempHum_initDriver();
	ASSERT_EQ(hih8120_initialise_fake.call_count, 1);
}

TEST_F(TempHumTest, temperature_unsuccesfull_init) {
	hih8120_initialise_fake.return_val = HIH8120_DRIVER_NOT_INITIALISED;

	tempHum_initDriver();
	ASSERT_EQ(hih8120_initialise_fake.call_count, 5);
}

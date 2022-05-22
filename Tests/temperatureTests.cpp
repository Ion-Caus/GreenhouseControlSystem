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


class TemperatureTest : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(hih8120_initialise);
	}
	void TearDown() override {}
};


TEST_F(TemperatureTest, test_get_temperature) {
	getTemperature();
}

TEST_F(TemperatureTest, test_succesfull_init) {
	hih8120_initialise_fake.return_val = HIH8120_OK;
	createTemperatureTask();
	ASSERT_EQ(hih8120_initialise_fake.call_count, 1);
}


#include "gtest/gtest.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

//extern "C" {
//#include <stdio.h>
//
//	// Header file from the production code project
//}
//
//// --- Create Mocks ---
//// void utils_initialise();
//FAKE_VOID_FUNC(utils_initialise);
//// int8_t utils_add(int8_t n1, int8_t n2);
//FAKE_VALUE_FUNC(int8_t, utils_add, int8_t, int8_t);
//// void utils_storeValue(int8_t value);
//FAKE_VOID_FUNC(utils_storeValue, int8_t);
//
//// ---------------------------------------------------------------------
//class MyTaskTest : public ::testing::Test {
//protected:
//	void SetUp() override {
//		RESET_FAKE(utils_initialise);
//		RESET_FAKE(utils_add);
//		RESET_FAKE(utils_storeValue);
//		RESET_FAKE(vTaskDelay);
//		RESET_FAKE(xSemaphoreGive);
//		FFF_RESET_HISTORY();
//	}
//
//	void TearDown() override {}
//};
//
//// ---------------------------------------------------------------------
//TEST_F(MyTaskTest, utils_initialise_calledDuringInitialisation) {
//	// Arrange
//	// Act
//	myTask_init();
//	// Assert
//	EXPECT_EQ(1, utils_initialise_fake.call_count);
//}
//
//// ---------------------------------------------------------------------
//TEST_F(MyTaskTest, vTaskDelay_calledCorrectFromTask) {
//	// Arrange
//	// Act
//	myTask_run();
//	// Assert
//	EXPECT_EQ(1, vTaskDelay_fake.call_count);  // Called once?
//	// Argument correct?
//	EXPECT_EQ(pdMS_TO_TICKS(200), vTaskDelay_fake.arg0_val);
//}
//
//// ---------------------------------------------------------------------
//TEST_F(MyTaskTest, utils_add_calledCorrectFromTask) {
//	// Arrange
//	// Act
//	myTask_run();
//	// Assert
//	EXPECT_EQ(1, utils_add_fake.call_count);  // Called once?
//	// Arguments correct?
//	EXPECT_EQ(20, utils_add_fake.arg0_val);
//	EXPECT_EQ(56, utils_add_fake.arg1_val);
//}
//
//// ---------------------------------------------------------------------
//TEST_F(MyTaskTest, utils_storeValue_calledWithResultOfCalculationFromTask) {
//	// Arrange
//	utils_add_fake.return_val = 12;
//	// Act
//	myTask_run();
//	// Assert
//	EXPECT_EQ(1, utils_storeValue_fake.call_count);  // Called once?
//	// Argument the same as the calculation result?
//	EXPECT_EQ(12, utils_storeValue_fake.arg0_val);
//}


/*
 * weighted_average_Tests.cpp
 *
 * Created: 6/22/2022 11:26:40 AM
 * Author: Deniss
 */

#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>


extern "C"
{
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "weighted_average.h"
}


class WeightedAverageTest : public ::testing::Test {
protected:
	void SetUp() override
	{
		RESET_FAKE(xSemaphoreCreateMutex);
		RESET_FAKE(xSemaphoreGive);
		RESET_FAKE(xSemaphoreTake);
	}
	void TearDown() override {}
};

TEST_F(WeightedAverageTest, test_weighted_average_mutex) {


	// Arrange
	int16_t samples[10] = { 0 };

	// Act
	int16_t weightedAverage = weightedAverage_calculate(samples, 10);

	// Assert
	ASSERT_EQ(xSemaphoreTake_fake.call_count, 1);
	ASSERT_EQ(xSemaphoreGive_fake.call_count, 1);
}



TEST_F(WeightedAverageTest, calculate_weighted_average_temp) {
	// Arrange
	int16_t samples[10] = { 230, 234, 232, 234, 290, 230, 232, 234, 234, 231 };

	// Act
	int16_t weightedAverage = weightedAverage_calculate(samples, 10);

	// Assert
	ASSERT_EQ(weightedAverage, 237); // 23.7 °C
}

TEST_F(WeightedAverageTest, calculate_weighted_average_hum) {
	// Arrange
	int16_t samples[10] = { 469, 458, 472, 969, 421, 455, 466, 464, 459, 467 };

	// Act
	int16_t weightedAverage = weightedAverage_calculate(samples, 10);

	// Assert
	ASSERT_EQ(weightedAverage, 485); // 48.5 %
}

TEST_F(WeightedAverageTest, calculate_weighted_average_co2) {
	// Arrange
	int16_t samples[10] = { 1000,2000,1100,1900,1200,1800,1700,1900,1400, 1000 };

	// Act
	int16_t weightedAverage = weightedAverage_calculate(samples, 10);

	// Assert
	ASSERT_EQ(weightedAverage, 1420); // ppm
}






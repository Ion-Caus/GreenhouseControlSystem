#include "gtest/gtest.h"

extern "C" {
	//#include <temperature.h>
}

class TemperatureTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};


TEST(TemperatureTest, Calculate_weightedAverageTemperature) {

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}
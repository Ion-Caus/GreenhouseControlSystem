#include "gtest/gtest.h"

class ProductionTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST(ProductionTest, Demo) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
#include "gtest/gtest.h"
#include <FreeRTOS_FFF_MocksDeclaration.h>


extern "C"
{
#include "co2.h"
#include "sensorConfig.h"
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

TEST_F(Co2Test, init_co2_driver) {
	co2_initDriver();
	ASSERT_EQ(mh_z19_initialise_fake.call_count, 1);
}

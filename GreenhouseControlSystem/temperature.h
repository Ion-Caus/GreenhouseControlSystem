#pragma once

#include <stdint.h>

void createTemperatureTask(void);
int16_t calculateWeightedAverage(int16_t array[], uint8_t size);
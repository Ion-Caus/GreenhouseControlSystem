#pragma once

#include <stdint.h>

int16_t getTemperature();
uint16_t getHumidity();

void initTempHumDriver();
void createTemperatureHumidityTask(void);
//int16_t calculateWeightedAverage(int16_t array[], uint8_t size);
void temperatureHumidity_destroy();
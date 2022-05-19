#pragma once

#include <stdint.h>

int16_t getTemperature();
uint16_t getHumidity();

void initTempHumDriver();
void createTemperatureHumidityTask(void);
void temperatureHumidity_destroy();
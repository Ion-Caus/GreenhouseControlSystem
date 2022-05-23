#pragma once

#include <stdint.h>

int16_t getTemperature();
uint16_t getHumidity();

void createTemperatureHumidityTask(void);
void temperatureHumidity_destroy(void);

// for testing
void initTempHumDriver();
void tempHumTask_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index);
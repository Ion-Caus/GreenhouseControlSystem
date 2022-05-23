#pragma once

#include <stdint.h>

int16_t getTemperature();
uint16_t getHumidity();

int16_t calculateWeightedAverage(int16_t arr[], uint8_t size);
void createTemperatureHumidityTask(void);
void temperatureHumidity_destroy(void);

// for testing
void initTempDriver();
void tempHumTask_run(int16_t* temperatureArray, int16_t* humidityArray, uint8_t* index);

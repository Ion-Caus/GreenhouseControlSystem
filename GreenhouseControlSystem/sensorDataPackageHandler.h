/*
 * sensorDataPackageHandle.h
 *
 * Created: 04/05/2022 22.28.01
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>
#include "lora_driver.h"

typedef struct measurements {
	int16_t temperature;
	uint16_t humidity;
	uint16_t co2;
} measurements_t;

void setTemperature(int16_t temp);
void setHumidity(uint16_t hum);
void setCO2(uint16_t co2);

measurements_t getSensorData();
/*
 * sensorDataPackageHandle.h
 *
 * Created: 04/05/2022 22.28.01
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>
#include "lora_driver.h"

void setTemperature(int16_t temp);

uint8_t* getArrPayload();
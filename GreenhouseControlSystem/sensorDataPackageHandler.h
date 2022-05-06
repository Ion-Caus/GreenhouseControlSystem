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

lora_driver_payload_t getLoRaPayload(uint8_t port_no);
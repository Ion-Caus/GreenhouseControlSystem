/*
 * sensorDataPackageHandle.h
 *
 * Created: 04/05/2022 22.28.01
 *  Author: Deniss
 */ 


#pragma once

#include <stdint.h>
#include "moisture.h"
#include "lora_driver.h"


typedef struct measurements {
	int16_t temperature;
	uint16_t humidity;
	uint16_t co2;
	uint8_t moistures[POT_COUNT];
	uint8_t status;
} measurements_t;


void sensorDataPackage_setTemperature(int16_t temp);
void sensorDataPackage_setHumidity(uint16_t hum);
void sensorDataPackage_setCO2(uint16_t co2);
void sensorDataPackage_setMoistures(uint8_t* moistures);


/*
 * Get the sensor package containing all the measurements
 */
measurements_t sensorDataPackage_getSensorData(void);

/*
 * Resets all the measurements to 0
 */
void sensorDataPackage_reset(void);
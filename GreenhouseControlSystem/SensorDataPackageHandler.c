/*
 * sensorDataPackageHandler.c
 *
 * Created: 04/05/2022 21.48.34
 *  Author: Deniss
 */ 

#include "sensorDataPackageHandler.h"

	
static measurements_t sensordata;


void sensorDataPackage_setTemperature(int16_t temp)
{
	sensordata.temperature = temp;
}

void sensorDataPackage_setHumidity(uint16_t hum)
{
	sensordata.humidity = hum;
}

void sensorDataPackage_setCO2(uint16_t co2ppm)
{
	sensordata.co2 = co2ppm;
}

void sensorDataPackage_setMoistures(uint8_t* moistures){
	
	for(int i = 0; i < POT_COUNT; i++){
		sensordata.moistures[i] = moistures[i];
	}
	
}

void sensorDataPackage_setStatus(uint8_t status){
	sensordata.status = status;
}

measurements_t sensorDataPackage_getSensorData()
{
	return sensordata;
}

void sensorDataPackage_reset()
{
	sensordata.temperature = 0;
	sensordata.humidity = 0;
	sensordata.co2 = 0;
	*sensordata.moistures = 0;
	sensordata.status = 0;
}

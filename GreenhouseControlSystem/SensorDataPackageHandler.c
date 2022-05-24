/*
 * SensorDataPackageHandler.c
 *
 * Created: 04/05/2022 21.48.34
 *  Author: Deniss
 */ 

#include "sensorDataPackageHandler.h"

	
static measurements_t sensordata;


void setTemperature(int16_t temp)
{
	sensordata.temperature = temp;
}

void setHumidity(uint16_t hum)
{
	sensordata.humidity = hum;
}

void setCO2(uint16_t co2ppm)
{
	sensordata.co2 = co2ppm;
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
}

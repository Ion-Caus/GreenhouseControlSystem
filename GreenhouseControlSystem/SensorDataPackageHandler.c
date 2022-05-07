/*
 * SensorDataPackageHandler.c
 *
 * Created: 04/05/2022 21.48.34
 *  Author: Deniss
 */ 

#include <stdint.h>
#include "sensorDataPackageHandler.h"


static int16_t temperature;

void setTemperature(int16_t temp){
	temperature = temp;
}

uint8_t* getArrPayload(){
	uint8_t array_payload[6];
	
	array_payload[0] = temperature >> 8;
	array_payload[1] = temperature & 0xFF;
	
	
	return array_payload;
}
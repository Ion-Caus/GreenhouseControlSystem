/*
 * SensorDataPackageHandler.c
 *
 * Created: 04/05/2022 21.48.34
 *  Author: Deniss
 */ 

#include <stdint.h>
#include "sensorDataPackageHandler.h"
#include "payloadConfig.h"


static int16_t temperature;
static uint16_t humidity;
static uint16_t CO2;

static uint8_t array_payload[UPLINK_PAYLOAD_LENGHT] = {0};

//TODO: refactor to a struct instead of array	
//typedef struct measurements {
	//int16_t temperature;
	//uint16_t humidity;
	//uint16_t CO2;
	//} measurements_t;
	//
//static measurements_t sensorData;

void setTemperature(int16_t temp){
	temperature = temp;
}

void setHumidity(uint16_t hum){
	humidity = hum;
}

void setCO2(uint16_t co2){
	CO2 = co2;
}

uint8_t* getArrPayload(){
	array_payload[0] = temperature >> 8;
	array_payload[1] = temperature & 0xFF;
	array_payload[2] = humidity >> 8;
	array_payload[3] = humidity & 0xFF;
	array_payload[4] = CO2 >> 8;
	array_payload[5] = CO2 & 0xFF;
	
	return array_payload;
}
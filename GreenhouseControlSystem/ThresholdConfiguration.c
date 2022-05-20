/*
 * ThresholdConfiguration.c
 *
 * Created: 20/05/2022 10.55.44
 *  Author: Deniss
 */ 

#include <stdint.h>
#include "ATMEGA_FreeRTOS.h"
#include <semphr.h>

static int16_t temperatureThreshold;
static uint16_t humidityThreshold;
static uint16_t CO2Threshold;

SemaphoreHandle_t lock;

void createThresholdConfig(){
	lock = xSemaphoreCreateMutex();	
}

int16_t getTemperatureThreshold(){
	
	int16_t temp = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temp = temperatureThreshold;
	}
	
	xSemaphoreGive(lock);
	return temp;
}

uint16_t getHumidityThreshold(){
	
	uint16_t hum = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		hum = humidityThreshold;
	}
	
	xSemaphoreGive(lock);
	return hum;
}

uint16_t getCo2Threshold(){
	uint16_t co2 = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		co2 = CO2Threshold;
	}
	
	xSemaphoreGive(lock);
	return co2;
}

void setTempThreshold(int16_t tempThreshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temperatureThreshold = tempThreshold;
	}
	xSemaphoreGive(lock);
}

void setHumThreshold(uint16_t humThreshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		humidityThreshold = humThreshold;
	}
	xSemaphoreGive(lock);
	
}
void setCo2Threshold(uint16_t co2Threshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		CO2Threshold = co2Threshold;
	}
	xSemaphoreGive(lock);
	
}

/*
 * ThresholdConfiguration.c
 *
 * Created: 20/05/2022 10.55.44
 *  Author: Deniss
 */ 

#include <stdint.h>
#include "ATMEGA_FreeRTOS.h"
#include <semphr.h>
#include <limits.h>

static int16_t temperatureThresholdUpper;
static int16_t temperatureThresholdLower;
static uint16_t CO2ThresholdUpper;
static uint16_t CO2ThresholdLower;

SemaphoreHandle_t lock;

void thresholdMutex_create(){
	lock = xSemaphoreCreateMutex();	
	temperatureThresholdLower = SHRT_MIN;
	temperatureThresholdUpper = SHRT_MAX;
	CO2ThresholdLower = 0;
	CO2ThresholdUpper = USHRT_MAX;
}

int16_t getTemperatureThresholdUpper(){
	
	int16_t temp = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temp = temperatureThresholdUpper;
	}
	
	xSemaphoreGive(lock);
	return temp;
}

int16_t getTemperatureThresholdLower(){
	
	int16_t temp = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temp = temperatureThresholdLower;
	}
	
	xSemaphoreGive(lock);
	return temp;
}

uint16_t getCo2ThresholdUpper(){
	uint16_t co2 = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		co2 = CO2ThresholdUpper;
	}
	
	xSemaphoreGive(lock);
	return co2;
}

uint16_t getCo2ThresholdLower(){
	uint16_t co2 = 0;
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		co2 = CO2ThresholdLower;
	}
	
	xSemaphoreGive(lock);
	return co2;
}

void setTempThresholdUpper(int16_t tempThreshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temperatureThresholdUpper = tempThreshold;
	}
	xSemaphoreGive(lock);
}

void setTempThresholdLower(int16_t tempThreshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		temperatureThresholdLower = tempThreshold;
	}
	xSemaphoreGive(lock);
}


void setCo2ThresholdUpper(uint16_t co2Threshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		CO2ThresholdUpper = co2Threshold;
	}
	xSemaphoreGive(lock);
}

void setCo2ThresholdLower(uint16_t co2Threshold){
	
	if(xSemaphoreTake(lock, portMAX_DELAY) == pdTRUE){
		CO2ThresholdLower = co2Threshold;
	}
	xSemaphoreGive(lock);
}
/*
 * sensorConfig.h
 *
 * Created: 29/05/2022 16.26.26
 *  Author: alex
 */ 


#pragma once

// temperature and humidity sensor
#define MIN_TEMPERATURE				(-20 * 10)
#define MAX_TEMPERATUE				(60  * 10)

#define MAX_HUMIDITY				(100 * 10) // 100%

#define TEMP_HUM_ARRAY_SIZE			(10)

// co2 sensor
#define MAX_CO2						( 5000 ) // max ppm

#define CO2_ARRAY_SIZE				( 10 )

// fake moisture sensor
#define POT_COUNT					(6)
#define POT_DELAY_MS				(80)

// lower number means the change is faster and vice versa
#define CHANGE_SPEED_INVERSE		(40)	// number higher then 200 will make the temperatures not change at all

#define TOP_MOISTURE				(70 - 1) // the minus one is for buffer
#define LOW_MOISTURE				(40 + 1) // the plus one is for buffer

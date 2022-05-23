/*
 * moisture.h
 *
 * Created: 5/18/2022 1:36:07 PM
 *  Author: Lukas
 */ 

#include <stdint.h>

#ifndef MOISTURE_H_
#define MOISTURE_H_

#define MOIST_TASK_STACK			( configMINIMAL_STACK_SIZE )
#define MOIST_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )

//these two methods are used outside of this method
void moisture_create();
uint8_t* moisture_getMoistures();

void moisture_createTask(void);
void moisture_destroy();

//method to be used for testing 
static uint8_t _fake_moisture_measurement(uint8_t previousMeasurement);
void moisture_taskRun(void);

#endif /* MOISTURE_H_ */
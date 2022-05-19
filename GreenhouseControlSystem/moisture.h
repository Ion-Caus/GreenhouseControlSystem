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
uint16_t* moisture_getMoistures();

void moisture_createTask(void);
void moisture_destroy();

#endif /* MOISTURE_H_ */
/*
 * co2.h
 *
 * Created: 18/05/2022 19.12.12
 *  Author: alex
 */ 

#pragma once

#include <stdint.h>

uint16_t getCo2();

void initCo2Driver();
void createCo2Task(void);


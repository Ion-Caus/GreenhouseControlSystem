/*
 * upLinkHandler_LoraWAN.h
 *
 * Created: 5/4/2022 9:52:23 PM
 *  Author: ionc
 */ 

#pragma once

#include <stdint.h>

/**
 * Create the UplinkHandler task and hardware reset of LoRaWAN
 */
void upLinkHandler_task_create(void);

// for testing
void upLinkHandler_task_run(uint8_t* packageBuffer);

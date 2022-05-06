/*
 * SensorDataPackageHandler.c
 *
 * Created: 04/05/2022 21.48.34
 *  Author: Deniss
 */ 

#include <stdint.h>
#include "lora_driver.h"
#include "sensorDataPackageHandler.h"


#include "payloadConfig.h"


static int16_t temperature;

void setTemperature(int16_t temp){
	temperature = temp;
}

lora_driver_payload_t getLoRaPayload(uint8_t port_no){
	lora_driver_payload_t uplink_payload;
	
	uplink_payload.len = UPLINK_PAYLOAD_LENGHT;
	uplink_payload.portNo = port_no;
	
	uplink_payload.bytes[0] = temperature >> 8;
	uplink_payload.bytes[1] = temperature & 0xFF;
	
	
	return uplink_payload;
}
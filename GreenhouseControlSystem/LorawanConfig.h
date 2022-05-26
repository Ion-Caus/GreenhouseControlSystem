/*
 * loraConfig.h
 *
 * Created: 4/28/2022 8:57:22 AM
 *  Author: lukis, ionc
 */ 


#ifndef GREENHOUSECONFIG_H_
#define GREENHOUSECONFIG_H_

#define LORA_appEUI "05ABE2835032EC3E"
#define LORA_appKEY "B90973872CFD40F5E380185AD43FC18C"

#define UPLINK_PAYLOAD_LEN			(sizeof(measurements_t))
#define UPLINK_PAYLOAD_PORTNO		(6)

#define DOWNLINK_COM_PORT			(1)

#endif /* GREENHOUSECONFIG_H_ */
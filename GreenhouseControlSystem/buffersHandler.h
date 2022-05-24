/*
 * buffers.h
 *
 * Created: 5/23/2022 11:33:09 PM
 *  Author: ionc
 */ 

#ifndef BUFFERSHANDLER_H_
#define BUFFERSHANDLER_H_

#include "message_buffer.h"

/**
 * The message buffer used by the WindowController task,
 * that receives the latest sensor data package from the application task
 */
MessageBufferHandle_t windowBuffer;

/**
 * The message buffer used by the UplinkHandler task to create the Lora payload
 * and receives the latest sensor data package from the application task
 */
MessageBufferHandle_t upLinkBuffer;

/**
 * The message buffer used by the DownlinkHandler task,
 * that receives the thresholds from LoraWAN
 */
MessageBufferHandle_t downLinkBuffer;


/**
 * Create the buffers: windowBuffer, uplinkBuffer and downlinkBuffer.
 */
void buffersHandler_create(void);

#endif // BUFFERSHANDLER_H_
/*
 * actual_data_provider.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "actual_data_provider.h"
#include "fifo_queue.h"
#include "can_receiver_driver.h"
#include "main.h"

#define BUFFER_SIZE		64

static volatile CanRxMsgTypeDef msgTabBuffer[BUFFER_SIZE];
static volatile FIFOQueue msgQueue;
static volatile uint16_t actualDataStorage[DATA_CHANNELS_NUMBER];

#define STOP_CAN_BYTE_OFFSET	0
#define STOP_BYTE_LENGTH		1
#define STOP_OFFSET				6

void ActualDataProvider_init(){
	FIFOQueue_init(&msgQueue, msgTabBuffer, BUFFER_SIZE);
	CAN_ReceiverDriver_init(&msgQueue);
}

uint16_t ActualDataProvider_getValue(Channel_TypeDef channel){
	return actualDataStorage[channel];
}

void ActualDataProvider_setValue(Channel_TypeDef channel, uint16_t data){
	actualDataStorage[channel] = dataHigher;
}

uint16_t ActualDataProvider_getOffset(Channel_TypeDef channel){

	int16_t ret;

	switch (channel){
	case STOP_DATA_CHANNEL:
		ret = STOP_OFFSET;
		break;
	}

	return ret;

}

static void ActualDataProvider_saveData(CanRxMsgTypeDef* pMsg, Channel_TypeDef channel, uint8_t byteOffset, uint8_t byteLength){

	if ( ((uint8_t) pMsg->DLC) < (byteOffset + byteLength) ){
		_Warning_Handler(__FILE__, __LINE__);
		return;
	}

	uint16_t dataHigher = pMsg->Data[byteOffset];
	uint16_t dataLower;
	if (byteLength == 1) {
		actualDataStorage[channel] = dataHigher;
	} else if (byteLength==2){
		dataLower = pMsg->Data[byteOffset+1];
		actualDataStorage[channel] = (dataHigher<<8);
		actualDataStorage[channel] |= (dataLower & 0x00FF);
	}

}

void ActualDataProvider_thread(){

	static CanRxMsgTypeDef msg;

	while (FIFOQueue_dequeue(&msgQueue, &msg) == FIFOStatus_OK){
		switch (msg.StdId) {
		case STOP_DATA_CHANNEL:
			ActualDataProvider_saveData(&msg, STOP_DATA_CHANNEL, STOP_CAN_BYTE_OFFSET, STOP_BYTE_LENGTH);
			break;
		}
	}


	CAN_ReceiverDriver_queueProcessedNotifier();

}


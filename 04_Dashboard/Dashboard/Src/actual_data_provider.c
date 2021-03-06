/*
 * actual_data_provider.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "actual_data_provider.h"
#include "fifo_queue.h"
#include "can_receiver_driver.h"
#include "rtc.h"
#include "main.h"

#define BUFFER_SIZE		64

static volatile CanRxMsgTypeDef msgTabBuffer[BUFFER_SIZE];
static volatile FIFOQueue msgQueue;
static volatile uint16_t actualDataStorage[DATA_CHANNELS_NUMBER];
static volatile RTC_TimeTypeDef dataArrivalTime[DATA_CHANNELS_NUMBER];
static volatile RTC_DateTypeDef dummyDate;

#define RPM_CAN_BYTE_OFFSET		0
#define RPM_CAN_BYTE_LENGTH		2
#define RPM_MULTIPLIER			1
#define RPM_DIVIDER				1
#define RPM_OFFSET				0

#define CLT_CAN_BYTE_OFFSET		6
#define CLT_CAN_BYTE_LENGTH		2
#define CLT_MULTIPLIER			1
#define CLT_DIVIDER				1
#define CLT_OFFSET				0

#define FUEL_LEVEL_CAN_BYTE_OFFSET		0
#define FUEL_LEVEL_CAN_BYTE_LENGTH		1
#define FUEL_LEVEL_MULTIPLIER			1
#define FUEL_LEVEL_DIVIDER				1
#define FUEL_LEVEL_OFFSET				0

#define OIL_PRESSURE_CAN_BYTE_OFFSET		4
#define OIL_PRESSURE_CAN_BYTE_LENGTH		1
#define OIL_PRESSURE_MULTIPLIER				16
#define OIL_PRESSURE_DIVIDER				1
#define OIL_PRESSURE_OFFSET					0

#define BATT_VOLTAGE_CAN_BYTE_OFFSET		2
#define BATT_VOLTAGE_CAN_BYTE_LENGTH		2
#define BATT_VOLTAGE_MULTIPLIER				37
#define BATT_VOLTAGE_DIVIDER				1
#define BATT_VOLTAGE_OFFSET					0

#define GEAR_CAN_BYTE_OFFSET		0
#define GEAR_CAN_BYTE_LENGTH		1
#define GEAR_MULTIPLIER				1
#define GEAR_DIVIDER				1
#define GEAR_OFFSET					0

void ActualDataProvider_init(){
	FIFOQueue_init(&msgQueue, msgTabBuffer, BUFFER_SIZE);
	CAN_ReceiverDriver_init(&msgQueue);

	RTC_TimeTypeDef dataTimeDefault;
	dataTimeDefault.Hours = 0;
	dataTimeDefault.Minutes = 0;
	dataTimeDefault.Seconds = 0;
	dataTimeDefault.TimeFormat = RTC_HOURFORMAT12_AM;
	dataTimeDefault.SubSeconds = 0;
	dataTimeDefault.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;

	for (uint8_t i=0; i<DATA_CHANNELS_NUMBER; i++){
		dataArrivalTime[i] = dataTimeDefault;
	}
}

RTC_TimeTypeDef ActualDataProvider_getDataArrivalTime(Channel_TypeDef channel){
	return dataArrivalTime[channel];
}

uint16_t ActualDataProvider_getValue(Channel_TypeDef channel){
	return actualDataStorage[channel];
}

uint16_t ActualDataProvider_getMultiplier(Channel_TypeDef channel){

	int16_t ret;

	switch (channel){
	case RPM_DATA_CHANNEL:
		ret = RPM_MULTIPLIER;
		break;
	case CLT_DATA_CHANNEL:
		ret = CLT_MULTIPLIER;
		break;
	case FUEL_LEVEL_DATA_CHANNEL:
		ret = FUEL_LEVEL_MULTIPLIER;
		break;
	case OIL_PRESSURE_DATA_CHANNEL:
		ret = OIL_PRESSURE_MULTIPLIER;
		break;
	case BATT_VOLTAGE_DATA_CHANNEL:
		ret = BATT_VOLTAGE_MULTIPLIER;
		break;
	case GEAR_DATA_CHANNEL:
		ret = GEAR_MULTIPLIER;
		break;
	default:
		ret = 1;
	}

	return ret;

}

uint16_t ActualDataProvider_getDivider(Channel_TypeDef channel){

	int16_t ret;

	switch (channel){
	case RPM_DATA_CHANNEL:
		ret = RPM_DIVIDER;
		break;
	case CLT_DATA_CHANNEL:
		ret = CLT_DIVIDER;
		break;
	case FUEL_LEVEL_DATA_CHANNEL:
		ret = FUEL_LEVEL_DIVIDER;
		break;
	case OIL_PRESSURE_DATA_CHANNEL:
		ret = OIL_PRESSURE_DIVIDER;
		break;
	case BATT_VOLTAGE_DATA_CHANNEL:
		ret = BATT_VOLTAGE_DIVIDER;
		break;
	case GEAR_DATA_CHANNEL:
		ret = GEAR_DIVIDER;
		break;
	default:
		ret = 1;
	}

	return ret;

}

uint16_t ActualDataProvider_getOffset(Channel_TypeDef channel){

	int16_t ret;

	switch (channel){
	case RPM_DATA_CHANNEL:
		ret = RPM_OFFSET;
		break;
	case CLT_DATA_CHANNEL:
		ret = CLT_OFFSET;
		break;
	case FUEL_LEVEL_DATA_CHANNEL:
		ret = FUEL_LEVEL_OFFSET;
		break;
	case OIL_PRESSURE_DATA_CHANNEL:
		ret = OIL_PRESSURE_OFFSET;
		break;
	case BATT_VOLTAGE_DATA_CHANNEL:
		ret = BATT_VOLTAGE_OFFSET;
		break;
	case GEAR_DATA_CHANNEL:
		ret = GEAR_OFFSET;
		break;
	default:
		ret = 1;
	}

	return ret;

}

static void ActualDataProvider_saveData(CanRxMsgTypeDef* pMsg, Channel_TypeDef channel, uint8_t byteOffset, uint8_t byteLength){

	if ( ((uint8_t) pMsg->DLC) < (byteOffset + byteLength) ){
		_Warning_Handler(__FILE__, __LINE__);
		return;
	}

	uint16_t dataLower = pMsg->Data[byteOffset];
	uint16_t dataHigher;
	if (byteLength == 1) {
		actualDataStorage[channel] = dataLower;
	} else if (byteLength==2){
		dataHigher = pMsg->Data[byteOffset+1];
		actualDataStorage[channel] = (dataHigher<<8);
		actualDataStorage[channel] |= (dataLower & 0x00FF);
	}

	HAL_RTC_GetTime(&hrtc, (RTC_TimeTypeDef*)&(dataArrivalTime[channel]), RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, (RTC_DateTypeDef*)&dummyDate, RTC_FORMAT_BIN);

}

void ActualDataProvider_thread(){

	static CanRxMsgTypeDef msg;

	while (FIFOQueue_dequeue(&msgQueue, &msg) == FIFOStatus_OK){
		switch (msg.StdId) {
		case RPM_CAN_CHANNEL:
			ActualDataProvider_saveData(&msg, RPM_DATA_CHANNEL, RPM_CAN_BYTE_OFFSET, RPM_CAN_BYTE_LENGTH);
			break;
		case CLT_OIL_PRESSURE_CAN_CHANNEL:
			ActualDataProvider_saveData(&msg, CLT_DATA_CHANNEL, CLT_CAN_BYTE_OFFSET, CLT_CAN_BYTE_LENGTH);
			ActualDataProvider_saveData(&msg, OIL_PRESSURE_DATA_CHANNEL, OIL_PRESSURE_CAN_BYTE_OFFSET, OIL_PRESSURE_CAN_BYTE_LENGTH);
			break;
		case BATT_VOLTAGE_CAN_CHANNEL:
			ActualDataProvider_saveData(&msg, BATT_VOLTAGE_DATA_CHANNEL, BATT_VOLTAGE_CAN_BYTE_OFFSET, BATT_VOLTAGE_CAN_BYTE_LENGTH);
			break;
		case FUEL_LEVEL_CAN_CHANNEL:
			ActualDataProvider_saveData(&msg, FUEL_LEVEL_DATA_CHANNEL, FUEL_LEVEL_CAN_BYTE_OFFSET, FUEL_LEVEL_CAN_BYTE_LENGTH);
			break;
		case GEAR_CAN_CHANNEL:
			ActualDataProvider_saveData(&msg, GEAR_DATA_CHANNEL, GEAR_CAN_BYTE_OFFSET, GEAR_CAN_BYTE_LENGTH);
			break;
		}
	}

	CAN_ReceiverDriver_queueProcessedNotifier();

}


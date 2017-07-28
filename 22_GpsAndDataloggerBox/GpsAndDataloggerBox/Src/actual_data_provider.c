/*
 * actual_data_provider.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "actual_data_provider.h"
#include "can_receiver_driver.h"
#include "rtc.h"
#include "main.h"

#define DATA_CHANNELS_NUMBER	2048 // = 2^11

static volatile ActualDataProvider_Data_TypeDef actualDataStorage [DATA_CHANNELS_NUMBER];

static volatile RTC_DateTypeDef dummyDate;

void ActualDataProvider_init(){

	CAN_ReceiverDriver_init();

	RTC_TimeTypeDef dataTimeDefault;
	dataTimeDefault.Hours = 0;
	dataTimeDefault.Minutes = 0;
	dataTimeDefault.Seconds = 0;

	for (uint16_t i=0; i<DATA_CHANNELS_NUMBER; i++){
		actualDataStorage[i].DLC = 0;
		actualDataStorage[i].arrivalTime = dataTimeDefault;
	}
}

static void ActualDataProvider_saveData(CanRxMsgTypeDef* pMsg){

	static uint32_t msgId;

	msgId = pMsg->StdId;

	actualDataStorage[msgId].DLC = pMsg->DLC;

	for (uint8_t i=0; i<8; i++){
		actualDataStorage[msgId].Data[i] = pMsg->Data[i];
	}

	HAL_RTC_GetTime(&hrtc, (RTC_TimeTypeDef*)&(actualDataStorage[msgId].arrivalTime), RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, (RTC_DateTypeDef*)&dummyDate, RTC_FORMAT_BIN);

}

void ActualDataProvider_thread(){

	static CanRxMsgTypeDef tmpMsg;

	while (CAN_ReceiverDriver_getLastElement(&tmpMsg) == CANRDriverStatus_OK){
		ActualDataProvider_saveData(&tmpMsg);
	}

}

ActualDataProvider_Data_TypeDef ActualDataProvider_getData(uint32_t channelId){
	return actualDataStorage[channelId];
}


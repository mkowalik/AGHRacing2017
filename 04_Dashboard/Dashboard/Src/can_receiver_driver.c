/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_receiver_driver.h"

static CanRxMsgTypeDef rx_msg;

static volatile FIFOQueue* pMsgQueue;

uint8_t waitingForInterrupt = 0;

static void filtersConfiguration(){

	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterMaskIdLow = 		GEAR_CAN_CHANNEL;
	filterConfig.FilterMaskIdHigh = 	CLT_OIL_PRESSURE_CAN_CHANNEL;
	filterConfig.FilterMaskIdLow = 		BATT_VOLTAGE_CAN_CHANNEL;
	filterConfig.FilterMaskIdHigh = 	FUEL_LEVEL_CAN_CHANNEL;


	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

	// filterConfig keeps previous configuration in unchanged fields
	filterConfig.FilterNumber = 		1;

	filterConfig.FilterMaskIdLow = 		RPM_CAN_CHANNEL;
	filterConfig.FilterMaskIdHigh = 	0;
	filterConfig.FilterMaskIdLow = 		0;
	filterConfig.FilterMaskIdHigh = 	0;

	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

}

void CAN_ReceiverDriver_init(volatile FIFOQueue* pMsgQueueArg){

	pMsgQueue = pMsgQueueArg;

	filtersConfiguration();

	hcan.pRxMsg = &rx_msg;

	HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	waitingForInterrupt = 1;

}

void CAN_ReceiverDriver_receiveITHandler(){

	if (hcan.pRxMsg->IDE != CAN_USED_ID) return; //TODO nie return tylko olej i uruchom ponownie HAL_CAN_Receive_IT
	if (hcan.pRxMsg->RTR != CAN_RTR_DATA) return; //TODO tu te¿

	if (FIFOQueue_enqueue(pMsgQueue, *(hcan.pRxMsg))==FIFOStatus_OK){
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
		waitingForInterrupt = 1;
	} else {
		waitingForInterrupt = 0;
	}


}

void CAN_ReceiverDriver_queueProcessedNotifier(){

	if (waitingForInterrupt == 0) {
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
		waitingForInterrupt = 1;
	}

}

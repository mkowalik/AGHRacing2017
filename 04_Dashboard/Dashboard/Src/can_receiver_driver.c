/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_receiver_driver.h"
#include "can.h"

static CanRxMsgTypeDef rx_msg;

static volatile FIFOQueue* pMsgQueue;

uint8_t waitingForInterrupt = 0;

// Overriding weak function from stm32f0xx_hal_can.c file
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	CAN_ReceiverDriver_receiveITHandler();
}

static void filtersConfiguration(){

	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterMaskIdLow = 		GEAR_CAN_CHANNEL;				// channel list 0
	filterConfig.FilterMaskIdHigh = 	CLT_OIL_PRESSURE_CAN_CHANNEL;	// channel list 1
	filterConfig.FilterMaskIdLow = 		BATT_VOLTAGE_CAN_CHANNEL;		// channel list 2
	filterConfig.FilterMaskIdHigh = 	FUEL_LEVEL_CAN_CHANNEL;			// channel list 3


	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

	// filterConfig keeps previous configuration with unchanged fields
	filterConfig.FilterNumber = 		1;

	filterConfig.FilterMaskIdLow = 		RPM_CAN_CHANNEL;				// channel list 4
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

	if ((hcan.pRxMsg->IDE != CAN_USED_ID) || (hcan.pRxMsg->RTR != CAN_RTR_DATA)) {
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
		waitingForInterrupt = 1;
	} else if (FIFOQueue_enqueue(pMsgQueue, *(hcan.pRxMsg))==FIFOStatus_OK){
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

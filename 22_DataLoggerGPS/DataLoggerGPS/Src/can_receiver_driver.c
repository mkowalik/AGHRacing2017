/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_receiver.h"
#include "can.h"

static volatile CanRxMsgTypeDef rx_msg;

static volatile FIFOQueue* pMsgQueue;

static volatile uint32_t canError;
static volatile HAL_CAN_StateTypeDef canState;

// Overriding weak function from stm32f0xx_hal_can.c file
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	CAN_ReceiverDriver_receiveITHandler();
}

// Overriding weak function from stm32f0xx_hal_can.c file
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){
	canError = HAL_CAN_GetError(hcan);
}

static void filtersConfiguration(){

	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterIdLow = 			RPM_CAN_CHANNEL<<5;					// channel list 0
	filterConfig.FilterIdHigh = 		CLT_OIL_PRESSURE_CAN_CHANNEL<<5;	// channel list 1
	filterConfig.FilterMaskIdLow = 		BATT_VOLTAGE_CAN_CHANNEL<<5;		// channel list 2
	filterConfig.FilterMaskIdHigh = 	FUEL_LEVEL_CAN_CHANNEL<<5;			// channel list 3


	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

	// filterConfig keeps previous configuration with unchanged fields
	filterConfig.FilterNumber = 		1;

	filterConfig.FilterIdLow = 			GEAR_CAN_CHANNEL<<5;				// channel list 4
	filterConfig.FilterIdHigh = 		0;
	filterConfig.FilterMaskIdLow = 		0;
	filterConfig.FilterMaskIdHigh = 	0;

	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

}

void CAN_ReceiverDriver_init(volatile FIFOQueue* pMsgQueueArg){

	pMsgQueue = pMsgQueueArg;

	filtersConfiguration();

	hcan.pRxMsg = (CanRxMsgTypeDef*) &rx_msg;

	HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

}


void CAN_ReceiverDriver_receiveITHandler(){

	if ((hcan.pRxMsg->IDE != CAN_USED_ID) || (hcan.pRxMsg->RTR != CAN_RTR_DATA)) {
		// For some reason RTR Data (shouldn't be in our system)
		canState = HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

	} else if (FIFOQueue_enqueue(pMsgQueue, *(hcan.pRxMsg))==FIFOStatus_OK){
		canState = HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	}

}

void CAN_ReceiverDriver_queueProcessedNotifier(){

	canState = HAL_CAN_GetState(&hcan);
	if (canState == HAL_CAN_STATE_READY || canState == HAL_CAN_STATE_BUSY_TX) {
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	}

}

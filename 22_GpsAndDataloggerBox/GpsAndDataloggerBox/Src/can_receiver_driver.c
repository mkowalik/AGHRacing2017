/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_receiver_driver.h"
#include "can.h"
#include "fifo_queue.h"

#define CAN_USED_ID		CAN_ID_STD

static volatile CanRxMsgTypeDef rxMsg;

static volatile uint32_t canError;
static volatile HAL_CAN_StateTypeDef canState;

// filter nothing
static void filtersConfiguration(){

	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uC
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_32BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDMASK;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterIdLow = 			0;
	filterConfig.FilterIdHigh = 		0;
	filterConfig.FilterMaskIdLow = 		0;
	filterConfig.FilterMaskIdHigh = 	0;

	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

}

static volatile CanRxMsgTypeDef msgTabBuffer[BUFFER_SIZE];
static volatile FIFOQueue msgQueue;

void CAN_ReceiverDriver_init(){

	FIFOQueue_init(&msgQueue, msgTabBuffer, BUFFER_SIZE, sizeof(CanRxMsgTypeDef));

	filtersConfiguration();

	hcan.pRxMsg = (CanRxMsgTypeDef*) &rxMsg;

	HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

}


static void CAN_ReceiverDriver_receiveITHandler(){

	if ((hcan.pRxMsg->IDE != CAN_USED_ID) || (hcan.pRxMsg->RTR != CAN_RTR_DATA)) {
		// For some reason RTR Data (shouldn't be in our system)
		canState = HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

	} else if (FIFOQueue_enqueue(&msgQueue, (hcan.pRxMsg))==FIFOStatus_OK){
		canState = HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	}

}

static void CAN_ReceiverDriver_errorITHandler(){

	canState = HAL_CAN_GetState(&hcan);
	if (canState == HAL_CAN_STATE_READY || canState == HAL_CAN_STATE_BUSY_TX) { //TODO jak zresetowac error
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	}

}

CANRDriverStatus CAN_ReceiverDriver_getLastElement(CanRxMsgTypeDef* pRetMsg){

	static volatile CanRxMsgTypeDef tmpMsg;

	CANRDriverStatus ret;
	FIFOStatus status = FIFOQueue_dequeue(&msgQueue, (void*) &tmpMsg);

	if (status==FIFOStatus_OK){
		*pRetMsg = tmpMsg;
		ret = CANRDriverStatus_OK;
	} else {
		ret = CANRDriverStatus_Empty;
	}

	canState = HAL_CAN_GetState(&hcan);
	if (canState == HAL_CAN_STATE_READY || canState == HAL_CAN_STATE_BUSY_TX) {
		HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
	}

	return ret;

}

// Overriding weak function from stm32f1xx_hal_can.c file
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	CAN_ReceiverDriver_receiveITHandler();
}

// Overriding weak function from stm32f1xx_hal_can.c file
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){
	CAN_ReceiverDriver_errorITHandler();
}

/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_transmitter_driver.h"
#include "can.h"

static volatile CanTxMsgTypeDef tx_msg;

static volatile FIFOQueue* pMsgQueue;

static volatile uint8_t waitingForInterrupt = 0;

// Overriding weak function from stm32f0xx_hal_can.c file
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan){
	CAN_TransmitterDriver_transmittITHandler();
}

void CAN_TransmitterDriver_init(volatile FIFOQueue* pMsgQueueArg){

	pMsgQueue = pMsgQueueArg;

	hcan.pTxMsg = (CanTxMsgTypeDef*) &tx_msg;

	CAN_Transmit_IT(&hcan, CAN_FIFO1);
	waitingForInterrupt = 1;

}

uint32_t counter = 0;
HAL_StatusTypeDef status;

void CAN_TransmitterDriver_transmittITHandler(){

	counter++;

	if ((hcan.pRxMsg->IDE != CAN_USED_ID) || (hcan.pRxMsg->RTR != CAN_RTR_DATA)) {
		status = CAN_Transmit_IT(&hcan, CAN_FIFO1);
		waitingForInterrupt = 1;
	} else if (FIFOQueue_dequeue(pMsgQueue, *(hcan.pTxMsg))==FIFOStatus_OK){
		status = CAN_Transmit_IT(&hcan, CAN_FIFO1);
		waitingForInterrupt = 1;
	} else {
		waitingForInterrupt = 0;
	}

}

void CAN_TransmitterDriver_queueProcessedNotifier(){

	if (waitingForInterrupt == 0) {
		CAN_Transmit_IT(&hcan, CAN_FIFO1);
		waitingForInterrupt = 1;
	}

}

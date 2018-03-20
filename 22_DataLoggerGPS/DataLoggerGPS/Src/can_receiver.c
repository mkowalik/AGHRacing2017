/*
 * can_receiver_driver.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "can_receiver.h"
#include "can.h"
#include "math.h"
#include "string.h"
#include "error_logger.h"
#include "stdio.h"

CANReceiver_TypeDef	canReceiver;
CANReceiver_TypeDef* pSelf = &canReceiver;	//TODO trzeba przerobic na wersje z klasa jako pierwszy argument funkcji

CANData_TypeDef pReceiverQueueBuffer [CAN_MSG_QUEUE_SIZE];

/** FUNCTIONS DECLARATIONS **/

static CANReceiver_Status_TypeDef CANReceiver_filtersConfiguration(Config_TypeDef* pConfig);

/** FUNCTIONS IMPLEMENTATIONS **/

CANReceiver_Status_TypeDef CANReceiver_init(Config_TypeDef* pConfig, CAN_HandleTypeDef* hcan, MSTimerMiddleware_TypeDef* pMSTimerMiddlewareHandler){

	CANReceiver_Status_TypeDef status1;

	pSelf->phcan = hcan;
	pSelf->pMSTimerMiddlewareHandler = pMSTimerMiddlewareHandler;

	FIFOStatus status2 = FIFOQueue_init(&(pSelf->framesFIFO), pReceiverQueueBuffer, sizeof(CANData_TypeDef), CAN_MSG_QUEUE_SIZE);	//TODO czy alignment nie popusje sizeof
																											//TODO czy nie lepiej przeniesc inicjalizacje wyzej?

	if (status2 != FIFOStatus_OK){
		return CANReceiver_Status_Error;
	}

	if((status1 = CANReceiver_filtersConfiguration(pConfig)) != CANReceiver_Status_OK){
		return status1;
	}

	hcan->pRxMsg = (CanRxMsgTypeDef*) &(pSelf->rxHALMsg);

	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
		return CANReceiver_Status_Error;
	}

	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
		return CANReceiver_Status_Error;
	}

	return CANReceiver_Status_OK;

}

CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANData_TypeDef* pRetMsg){

	FIFOStatus fifoStatus = FIFOStatus_OK;

	fifoStatus=FIFOQueue_dequeue(&(pSelf->framesFIFO), pRetMsg);

	switch(fifoStatus){
		case FIFOStatus_OK:
			return CANReceiver_Status_OK;
		case FIFOStatus_Empty:
			return CANReceiver_Status_Empty;
		case FIFOStatus_Error:
		default:
			return CANReceiver_Status_Error;
	}

	return CANReceiver_Status_OK;

}

static CANReceiver_Status_TypeDef CANReceiver_filtersConfiguration(Config_TypeDef* pConfig){

	//TODO zrobic obsluge wiekszej ilosci filtrow niz 14*4

	CANReceiver_Status_TypeDef retStatus = CANReceiver_Status_OK;

	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc, second bank only for sharing with slave CAN if exists
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	for(uint16_t i=0; i<pConfig->num_of_frames; i+=CAN_FRAMES_PER_FILTER){

		if (i>CAN_FILTERS_NUMBER * CAN_FRAMES_PER_FILTER){
			retStatus = CANReceiver_Status_TooManyFramesIDs;
			return retStatus;
		}

		filterConfig.FilterFIFOAssignment = i%2==0 ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;

		filterConfig.FilterNumber = 		i/CAN_FRAMES_PER_FILTER;	//each filter houses 4 IDs

		filterConfig.FilterIdLow = 			(i  <pConfig->num_of_frames) ? ((pConfig->frames[i  ].ID) << 5) : 0;			// channel list 0 //TODO kropka czy strza³ka???
		filterConfig.FilterIdHigh = 		(i+1<pConfig->num_of_frames) ? ((pConfig->frames[i+1].ID) << 5) : 0;			// channel list 1 //TODO kropka czy strza³ka???
		filterConfig.FilterMaskIdLow = 		(i+2<pConfig->num_of_frames) ? ((pConfig->frames[i+2].ID) << 5) : 0;			// channel list 2 //TODO kropka czy strza³ka???
		filterConfig.FilterMaskIdHigh = 	(i+3<pConfig->num_of_frames) ? ((pConfig->frames[i+3].ID) << 5) : 0;			// channel list 3 //TODO kropka czy strza³ka???

		if (HAL_CAN_ConfigFilter(pSelf->phcan, &filterConfig) != HAL_OK) {
			retStatus = CANReceiver_Status_Error;
			return retStatus;
		}

	}

	return retStatus;

}

static CANReceiver_Status_TypeDef receiveFromFIFO0(){

	if (pSelf->phcan->pRxMsg->IDE != CAN_ID_STD){
		return CANReceiver_Status_NotSTDFrame;
	}

	if (pSelf->phcan->pRxMsg->RTR != CAN_RTR_DATA) {
		return CANReceiver_Status_RTRFrame;
	}

	CANData_TypeDef tmpData;
	MSTimerMiddleware_Status_TypeDef status = MSTimerMiddleware_getMSTime(pSelf->pMSTimerMiddlewareHandler, &(tmpData.msTime));
	tmpData.DLC = pSelf->phcan->pRxMsg->DLC;
	memcpy(tmpData.Data, pSelf->phcan->pRxMsg->Data, 8);
	tmpData.ID = pSelf->phcan->pRxMsg->StdId;
//	pCanRecaiver->phcan->Instance->sFIFOMailBox[0]->RDTR & 0xFFFF0000; //st¹d mo¿na wziac message timestamp linia 1595 w stm32f1xx_hal_can.c, dokumentacja str 689

	if (status != MSTimerMiddleware_Status_OK){
		return CANReceiver_Status_RunTimeError;
	}

	if (FIFOQueue_enqueue(&(pSelf->framesFIFO), &tmpData) != FIFOStatus_OK){
		return CANReceiver_Status_RunTimeError;
	}

	return CANReceiver_Status_OK;
}


static CANReceiver_Status_TypeDef receiveFromFIFO1(){

	if (pSelf->phcan->pRxMsg->IDE != CAN_ID_STD){
		return CANReceiver_Status_NotSTDFrame;
	}

	if (pSelf->phcan->pRxMsg->RTR != CAN_RTR_DATA) {
		return CANReceiver_Status_RTRFrame;
	}

	static CANData_TypeDef tmpData;
	MSTimerMiddleware_Status_TypeDef status = MSTimerMiddleware_getMSTime(pSelf->pMSTimerMiddlewareHandler, &(tmpData.msTime));
	tmpData.DLC = pSelf->phcan->pRx1Msg->DLC;
	memcpy(tmpData.Data, pSelf->phcan->pRx1Msg->Data, 8);
	tmpData.ID = pSelf->phcan->pRx1Msg->StdId;
//	pCanRecaiver->phcan->Instance->sFIFOMailBox[1]->RDTR & 0xFFFF0000; //st¹d mo¿na wziac message timestamp linia 1595 w stm32f1xx_hal_can.c, dokumentacja str 689

	if (status != MSTimerMiddleware_Status_OK){
		return CANReceiver_Status_RunTimeError;
	}

	if (FIFOQueue_enqueue(&(pSelf->framesFIFO), &tmpData) != FIFOStatus_OK){
		return CANReceiver_Status_RunTimeError;
	}

	return CANReceiver_Status_OK;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){

	if (hcan != pSelf->phcan){
		logError(ERROR_CAN, "hcan != pSelf->phcan");
	}

	switch (HAL_CAN_GetState(pSelf->phcan)){
	case HAL_CAN_STATE_READY:
	case HAL_CAN_STATE_BUSY_TX:
		if (receiveFromFIFO0(pSelf) != CANReceiver_Status_OK){
			logError(ERROR_CAN_RECEIVE_FIFO0, "During RxCpltCallback");
		}
		if (receiveFromFIFO1(pSelf) != CANReceiver_Status_OK){
			logError(ERROR_CAN_RECEIVE_FIFO1, "During RxCpltCallback");
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO0, "During RxCpltCallback");
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO1, "During RxCpltCallback");
		}
		break;
	case HAL_CAN_STATE_BUSY_RX0:
	case HAL_CAN_STATE_BUSY_TX_RX0:
		if (receiveFromFIFO1(pSelf) != CANReceiver_Status_OK){
			logError(ERROR_CAN_RECEIVE_FIFO1, "During RxCpltCallback");
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO1, "During RxCpltCallback");
		}
		break;
	case HAL_CAN_STATE_BUSY_RX1:
	case HAL_CAN_STATE_BUSY_TX_RX1:
		if (receiveFromFIFO0(pSelf) != CANReceiver_Status_OK){
			logError(ERROR_CAN_RECEIVE_FIFO0, "During RxCpltCallback");
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO0, "During RxCpltCallback");
		}
		break;
	default:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO0, "During RxCpltCallback");
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			logError(ERROR_CAN_RECEIVE_FIFO1, "During RxCpltCallback");
		}
	}

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){

	static char errorTextBuffer[50];

	if (hcan != pSelf->phcan){
		logError(ERROR_CAN, "hcan != pSelf->phcan");
	}

	if (HAL_CAN_GetError(pSelf->phcan) & HAL_CAN_ERROR_EPV){ // Error passive
		logError(ERROR_CAN_EPV, "HAL_CAN_ERROR_EPV");
	} else if (HAL_CAN_GetError(pSelf->phcan) & HAL_CAN_ERROR_BOF){ // BUS Off
		logError(ERROR_CAN_BOF, "HAL_CAN_ERROR_BOF");
	} else if (HAL_CAN_GetError(pSelf->phcan) & HAL_CAN_ERROR_ACK){ // ACK Error
		logError(ERROR_CAN_ACK, "HAL_CAN_ERROR_ACK");
	} else if (HAL_CAN_GetError(pSelf->phcan) & HAL_CAN_ERROR_FOV0){ // FIFO 0 Overrun
		logError(ERROR_CAN_FOV0, "HAL_CAN_ERROR_FOV0");
	} else if (HAL_CAN_GetError(pSelf->phcan) & HAL_CAN_ERROR_FOV1){ // FIFO 1 Overrun
		logError(ERROR_CAN_FOV1, "HAL_CAN_ERROR_FOV1");
	}

	if (HAL_CAN_GetError(pSelf->phcan) != HAL_CAN_ERROR_NONE){
		snprintf(errorTextBuffer, 50, "HAL_CAN_ERROR: 0x%lx", HAL_CAN_GetError(pSelf->phcan));
		logError(ERROR_CAN, errorTextBuffer);
	}

	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
		logError(ERROR_CAN_RECEIVE_FIFO0, "During ErrorCallback");
	}
	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
		logError(ERROR_CAN_RECEIVE_FIFO1, "During ErrorCallback");
	}

}

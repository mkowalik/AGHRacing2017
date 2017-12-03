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

#define	CAN_FILTERS_NUMBER	14	//TODO a nie 14, a bank number to 28???

CANReceiver_TypeDef	canReceiver;
CANReceiver_TypeDef* pSelf = &canReceiver;

CANData_TypeDef pReceiverQueueBuffer [CAN_MSG_QUEUE_SIZE];

/** FUNCTIONS DECLARATIONS **/

static CANReceiver_Status_TypeDef CANReceiver_filtersConfiguration(Config_TypeDef* pConfig);

/** FUNCTIONS IMPLEMENTATIONS **/

CANReceiver_Status_TypeDef CANReceiver_init(Config_TypeDef* pConfig, CAN_HandleTypeDef* hcan){

	CANReceiver_Status_TypeDef retStatus = CANReceiver_Status_OK;

	pSelf->phcan = hcan;
	FIFOQueue_init(&(pSelf->framesFIFO), pReceiverQueueBuffer, sizeof(CANData_TypeDef), CAN_MSG_QUEUE_SIZE);	//TODO czy alignment nie popusje sizeof

	if((retStatus = CANReceiver_filtersConfiguration(pConfig)) != CANReceiver_Status_OK){
		return retStatus;
	}

	hcan->pRxMsg = (CanRxMsgTypeDef*) &(pSelf->rxHALMsg);

	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
		retStatus = CANReceiver_Status_Error;
		return retStatus;
	}

	if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
		retStatus = CANReceiver_Status_Error;
		return retStatus;
	}

	return retStatus;

}

CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANData_TypeDef* pRetMsg){

	FIFOStatus fifoStatus = FIFOStatus_OK;

	fifoStatus=FIFOQueue_dequeue(&(pSelf->framesFIFO), pRetMsg);

	switch(fifoStatus){
		case FIFOStatus_OK:
			return FIFOStatus_OK;
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

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	for(uint16_t i=0; i<pConfig->num_of_frames; i+=4){

		if (i>CAN_FILTERS_NUMBER * 4){
			retStatus = CANReceiver_Status_TooManyFramesIDs;
			return retStatus;
		}

		filterConfig.FilterFIFOAssignment = i%2==0 ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;

		filterConfig.FilterNumber = 		i/4;	//each filter houses 4 IDs

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

	if ((pSelf->phcan->pRxMsg->IDE != CAN_ID_STD) || (pSelf->phcan->pRxMsg->RTR != CAN_RTR_DATA)) {
		return CANReceiver_Status_OK;
	}

	static CANData_TypeDef tmpData;
	tmpData.DLC = pSelf->phcan->pRxMsg->DLC;
	memcpy(tmpData.Data, pSelf->phcan->pRxMsg->Data, 8);
	tmpData.ID = pSelf->phcan->pRxMsg->StdId;
//			pCanRecaiver->phcan->Instance->sFIFOMailBox[0]->RDTR & 0xFFFF;
	tmpData.preciseTime = PreciseTimeMiddleware_getPreciseTime();

	if (FIFOQueue_enqueue(&(pSelf->framesFIFO), &tmpData) != FIFOStatus_OK){
		return CANReceiver_Status_RunTimeError;
	}

	return CANReceiver_Status_OK;
}


static CANReceiver_Status_TypeDef receiveFromFIFO1(){

	if ((pSelf->phcan->pRx1Msg->IDE != CAN_ID_STD) || (pSelf->phcan->pRx1Msg->RTR != CAN_RTR_DATA)) {
		return CANReceiver_Status_OK;
	}

	static CANData_TypeDef tmpData;
	tmpData.DLC = pSelf->phcan->pRx1Msg->DLC;
	memcpy(tmpData.Data, pSelf->phcan->pRx1Msg->Data, 8);
	tmpData.ID = pSelf->phcan->pRx1Msg->StdId;
//			pCanRecaiver->phcan->Instance->sFIFOMailBox[1]->RDTR & 0xFFFF;
	tmpData.preciseTime = PreciseTimeMiddleware_getPreciseTime();

	if (FIFOQueue_enqueue(&(pSelf->framesFIFO), &tmpData) != FIFOStatus_OK){
		return CANReceiver_Status_RunTimeError;
	}

	return CANReceiver_Status_OK;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){

	//TODO mozna dopisac w bibliotece pRxMsg->TIME = (uint16_t)(0xFF00U & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR) linia 1595 w stm32f1xx_hal_can.c, dokumentacja str 689

	if (hcan != canReceiver.phcan){
		// TODO ERROR
	}

	switch (HAL_CAN_GetState(pSelf->phcan)){
	case HAL_CAN_STATE_READY:
	case HAL_CAN_STATE_BUSY_TX:
		if (receiveFromFIFO0(pSelf) != CANReceiver_Status_OK){
			// TODO error
		}
		if (receiveFromFIFO1(pSelf) != CANReceiver_Status_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
		break;
	case HAL_CAN_STATE_BUSY_RX0:
	case HAL_CAN_STATE_BUSY_TX_RX0:
		if (receiveFromFIFO1(pSelf) != CANReceiver_Status_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
		break;
	case HAL_CAN_STATE_BUSY_RX1:
	case HAL_CAN_STATE_BUSY_TX_RX1:
		if (receiveFromFIFO0(pSelf) != CANReceiver_Status_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		break;
	default:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
	}

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){

	if (hcan != canReceiver.phcan){
		// TODO ERROR
	}

	CANReceiver_TypeDef* pCanReceiver = &canReceiver;

	switch (HAL_CAN_GetState(pCanReceiver->phcan)){
	case HAL_CAN_STATE_READY:
	case HAL_CAN_STATE_BUSY_TX:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
		break;
	case HAL_CAN_STATE_BUSY_RX0:
	case HAL_CAN_STATE_BUSY_TX_RX0:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
		break;
	case HAL_CAN_STATE_BUSY_RX1:
	case HAL_CAN_STATE_BUSY_TX_RX1:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		break;
	default:
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO0) != HAL_OK){
			// TODO error
		}
		if (HAL_CAN_Receive_IT(pSelf->phcan, CAN_FIFO1) != HAL_OK){
			// TODO error
		}
	}

}

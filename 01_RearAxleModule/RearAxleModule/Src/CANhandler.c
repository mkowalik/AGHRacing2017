/*
 * CANhandler.c
 *
 *  Created on: Jul 13, 2017
 *      Author: Wojciech
 */


#ifdef __cplusplus
 extern "C" {
#endif

#include "can.h"
#include "CANhandler.h"

volatile CANhandler_handle_t handle;

CanRxMsgTypeDef rx_mes;
CanTxMsgTypeDef tx_mes;

volatile uint8_t intPending;


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){

		if ((hcan->pRxMsg->IDE == CAN_ID_STD) && (hcan->pRxMsg->RTR == CAN_RTR_DATA)) {

	#ifdef CAN_STOP_LIGHT_RECEIVE
			if(hcan->pRxMsg->StdId == FAM_ACC_STOP_SW_ID){
				handle.data[DATA_STOP_LIGHT_LOCAL_ID]->data = hcan->pRxMsg->Data[DATA_STOP_SW_BYTEOFFSET];
				handle.data[DATA_STOP_LIGHT_LOCAL_ID]->updateTime = HAL_GetTick();
			}
	#endif

		}
		HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan){

	intPending = 0;

	if(0){

	}
#if  defined(CAN_STOP_LIGHT_TRANSMIT) || defined(CAN_ACC_TRANSMIT)
	else if(handle.data[DATA_R_ACC_X_LOCAL_ID]->updateTime < HAL_GetTick() - RAM_ACC_PERIOD){
		handle.data[DATA_R_ACC_X_LOCAL_ID]->updateTime = HAL_GetTick();
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->StdId = RAM_ACC_ID;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 6;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_ACC_X_BYTEOFFSET]))) = handle.data[DATA_R_ACC_X_LOCAL_ID]->data;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_ACC_Y_BYTEOFFSET]))) = handle.data[DATA_R_ACC_Y_LOCAL_ID]->data;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_ACC_Z_BYTEOFFSET]))) = handle.data[DATA_R_ACC_Z_LOCAL_ID]->data;
		HAL_CAN_Transmit_IT(hcan);
		intPending = 1;
	}
#endif

#if  defined(CAN_GYR_TRANSMIT)
	else if(handle.data[DATA_R_GYR_Y_LOCAL_ID]->updateTime < HAL_GetTick() - RAM_GYRO_PERIOD){
		handle.data[DATA_R_GYR_Y_LOCAL_ID]->updateTime = HAL_GetTick();
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->StdId = RAM_GYRO_ID;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 6;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_GYR_X_BYTEOFFSET]))) = handle.data[DATA_R_GYR_X_LOCAL_ID]->data;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_GYR_Y_BYTEOFFSET]))) = handle.data[DATA_R_GYR_Y_LOCAL_ID]->data;
		*((uint16_t *)(&(hcan->pTxMsg->Data[DATA_R_GYR_Z_BYTEOFFSET]))) = handle.data[DATA_R_GYR_Z_LOCAL_ID]->data;
		HAL_CAN_Transmit_IT(hcan);
		intPending = 1;
	}
#endif

}

CAN_FilterConfTypeDef filterConfig;

void CANhandler_Init(void){

#if defined(CAN_STOP_LIGHT_TRANSMIT) || defined(CAN_STOP_LIGHT_RECEIVE)
	static CANhandler_data_t can_stop_light_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_STOP_LIGHT_LOCAL_ID] = &can_stop_light_value;
#endif

#if defined(CAN_GYR_TRANSMIT) || defined(CAN_GYR_RECEIVE)
	static CANhandler_data_t can_gyr_x_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_GYR_X_LOCAL_ID] = &can_gyr_x_value;

	static CANhandler_data_t can_gyr_y_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_GYR_Y_LOCAL_ID] = &can_gyr_y_value;

	static CANhandler_data_t can_gyr_z_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_GYR_Z_LOCAL_ID] = &can_gyr_z_value;
#endif

#if defined(CAN_ACC_TRANSMIT) || defined(CAN_ACC_RECEIVE)
	static CANhandler_data_t can_acc_x_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_ACC_X_LOCAL_ID] = &can_acc_x_value;

	static CANhandler_data_t can_acc_y_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_ACC_Y_LOCAL_ID] = &can_acc_y_value;

	static CANhandler_data_t can_acc_z_value = {.data = 0, .dataValid = 0, .updateTime = 0};
	handle.data[DATA_R_ACC_Z_LOCAL_ID] = &can_acc_z_value;
#endif


	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterIdLow = 			FAM_ACC_STOP_SW_ID<<5;
	filterConfig.FilterIdHigh = 		0;
	filterConfig.FilterMaskIdLow = 		0;
	filterConfig.FilterMaskIdHigh = 	0;


	HAL_CAN_ConfigFilter(&hcan, &filterConfig);

	hcan.pRxMsg = (CanRxMsgTypeDef*) &rx_mes;
	hcan.pTxMsg = (CanTxMsgTypeDef*) &tx_mes;
	HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
//	hcan.pTxMsg->IDE = CAN_ID_STD;
//	hcan.pTxMsg->StdId = FAM_DSI_ID;
//	hcan.pTxMsg->RTR = CAN_RTR_DATA;
//	hcan.pTxMsg->Data[0] = 0;
//	hcan.pTxMsg->Data[1] = 0;
//	hcan.pTxMsg->Data[2] = 0;
//	hcan.pTxMsg->Data[3] = 0;
//	hcan.pTxMsg->Data[4] = 0;
//	hcan.pTxMsg->Data[5] = 0;
//	hcan.pTxMsg->Data[6] = 0;
//	hcan.pTxMsg->Data[7] = 0;
//	HAL_CAN_Transmit_IT(&hcan);
	intPending = 0;
}

void CANhandler_Handler(void){

#ifdef CAN_STOP_LIGHT_RECEIVE
		handle.data[DATA_STOP_LIGHT_LOCAL_ID]->dataValid =
				(handle.data[DATA_STOP_LIGHT_LOCAL_ID]->updateTime > HAL_GetTick() - (FAM_ACC_STOP_SW_PERIOD)*10);
#endif

		if(intPending == 0){
#if  defined(CAN_STOP_LIGHT_TRANSMIT) || defined(CAN_ACC_TRANSMIT)
			if(handle.data[DATA_R_ACC_X_LOCAL_ID]->updateTime < HAL_GetTick() - RAM_ACC_PERIOD){
				handle.data[DATA_R_ACC_X_LOCAL_ID]->updateTime = HAL_GetTick();
				hcan.pTxMsg->IDE = CAN_ID_STD;
				hcan.pTxMsg->StdId = RAM_ACC_ID;
				hcan.pTxMsg->RTR = CAN_RTR_DATA;
				hcan.pTxMsg->DLC = 6;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_ACC_X_BYTEOFFSET]))) = handle.data[DATA_R_ACC_X_LOCAL_ID]->data;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_ACC_Y_BYTEOFFSET]))) = handle.data[DATA_R_ACC_Y_LOCAL_ID]->data;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_ACC_Z_BYTEOFFSET]))) = handle.data[DATA_R_ACC_Z_LOCAL_ID]->data;
				HAL_CAN_Transmit_IT(&hcan);
				intPending = 1;
			}
#endif

#if  defined(CAN_GYR_TRANSMIT)
			else if(handle.data[DATA_R_GYR_Y_LOCAL_ID]->updateTime < HAL_GetTick() - RAM_GYRO_PERIOD){
				handle.data[DATA_R_GYR_Y_LOCAL_ID]->updateTime = HAL_GetTick();
				hcan.pTxMsg->IDE = CAN_ID_STD;
				hcan.pTxMsg->StdId = RAM_GYRO_ID;
				hcan.pTxMsg->RTR = CAN_RTR_DATA;
				hcan.pTxMsg->DLC = 6;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_GYR_X_BYTEOFFSET]))) = handle.data[DATA_R_GYR_X_LOCAL_ID]->data;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_GYR_Y_BYTEOFFSET]))) = handle.data[DATA_R_GYR_Y_LOCAL_ID]->data;
				*((uint16_t *)(&(hcan.pTxMsg->Data[DATA_R_GYR_Z_BYTEOFFSET]))) = handle.data[DATA_R_GYR_Z_LOCAL_ID]->data;
				HAL_CAN_Transmit_IT(&hcan);
				intPending = 1;
			}
#endif
		}
}


#ifdef __cplusplus
}
#endif

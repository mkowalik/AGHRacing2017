/*
 * action_scheduler.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */


#include "action_scheduler.h"
#include "stdio.h"

static ActionScheduler_Status_TypeDef ActionScheduler_mainLoop(ActionScheduler_TypeDef* pSelf);
static uint8_t AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static uint8_t AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData);
static ActionScheduler_Status_TypeDef ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf);
static ActionScheduler_Status_TypeDef ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf);

ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager,
		DataSaverBuffer_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver, RTCMiddleware_TypeDef* pRTCMiddleware){

	if (pSelf->state != ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_Error;
	}

	pSelf->pConfigManager = pConfigManager;
	pSelf->pDataSaver = pDataSaver;
	pSelf->pCANReceiver = pCANReceiver;
	pSelf->pRTCMiddleware = pRTCMiddleware;
	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_mainLoop(ActionScheduler_TypeDef* pSelf){

	switch (pSelf->state){
	case ActionScheduler_State_UnInitialized:
	default:
		return ActionScheduler_Status_UnInitializedError;
		break;
	case ActionScheduler_State_Idle:
		return ActionScheduler_idleState(pSelf);
		break;
	case ActionScheduler_State_LogInit:
		return ActionScheduler_logInitState(pSelf);
		break;
	case ActionScheduler_State_Logging:
		return ActionScheduler_loggingState(pSelf);
		break;
	case ActionScheduler_State_LogClose:
		return ActionScheduler_logCloseState(pSelf);
		break;
	}

	return ActionScheduler_Status_Error;
}

static uint8_t AcionScheduler_StartLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if ((pData->ID == 0x600) &&
		((pData->Data[2] | ((pData->Data[3])<<8)) > 50)){
			return 1;
	}
	return 0;

}

static uint8_t AcionScheduler_StopLogTrigger(ActionScheduler_TypeDef* pSelf, CANData_TypeDef* pData){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if ((pData->ID == 0x600) &&
		((pData->Data[2] | ((pData->Data[3])<<8)) < 50)){
			return 1;
	}
	return 0;

}

static ActionScheduler_Status_TypeDef ActionScheduler_idleState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	CANData_TypeDef msg;
	CANReceiver_Status_TypeDef status;

	do {

		status = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &msg);

		if (status == CANReceiver_Status_OK){

			if (AcionScheduler_StartLogTrigger(pSelf, &msg) != 0){
				pSelf->state = ActionScheduler_State_LogInit;
				pSelf->logStartTime = msg.msTime;
				break;
			}

		} else if (status ==  CANReceiver_Status_Empty) {

			break;

		} else { //CANReceiver_Status_Error:

			return ActionScheduler_Status_Error;

		}

	} while(1);

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_logInitState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	DateTime_TypeDef dateTime;
	if (RTCMiddleware_getDateAndTime(pSelf->pRTCMiddleware, &dateTime) != RTCMiddleware_Status_OK){
		return ActionScheduler_Status_Error;
	}

	if (DataSaverBuffer_openFile(pSelf->pDataSaver, dateTime) != DataSaverBuffer_Status_OK){
		return ActionScheduler_Status_Error;
	}

	pSelf->state = ActionScheduler_State_Logging;

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_loggingState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	CANData_TypeDef msg;
	CANReceiver_Status_TypeDef status;

	do {

		status = CANReceiver_pullLastFrame(pSelf->pCANReceiver, &msg);

		if (status == CANReceiver_Status_OK){

			msg.msTime -= pSelf->logStartTime;

			if (AcionScheduler_StopLogTrigger(pSelf, &msg) != 0){
				pSelf->state = ActionScheduler_State_LogClose;
				break;
			}

			if (DataSaverBuffer_writeData(pSelf->pDataSaver, &msg) != DataSaverBuffer_Status_OK){
				return ActionScheduler_Status_Error;
			}

		} else if (status ==  CANReceiver_Status_Empty) {

			break;

		} else { //CANReceiver_Status_Error:

			return ActionScheduler_Status_Error;

		}

	} while(1);

	return ActionScheduler_Status_OK;

}

static ActionScheduler_Status_TypeDef ActionScheduler_logCloseState(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if (DataSaverBuffer_closeFile(pSelf->pDataSaver) != DataSaverBuffer_Status_OK){
		return ActionScheduler_Status_Error;
	}

	pSelf->state = ActionScheduler_State_Idle;

	return ActionScheduler_Status_OK;

}

ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf){

	if (pSelf->state == ActionScheduler_State_UnInitialized){
		return ActionScheduler_Status_UnInitializedError;
	}

	if (CANReceiver_start(pSelf->pCANReceiver) != CANReceiver_Status_OK){
		return ActionScheduler_Status_Error;
	}

	pSelf->state = ActionScheduler_State_Idle;

	while (ActionScheduler_mainLoop(pSelf) == ActionScheduler_Status_OK);

	return ActionScheduler_Status_Error;
}

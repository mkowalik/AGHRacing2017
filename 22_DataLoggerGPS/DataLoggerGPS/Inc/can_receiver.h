/*
 * can_receiver_driver.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "fifo_queue.h"
#include "can_data.h"
#include "config.h"
#include "ms_timer_middleware.h"

#ifndef CAN_RECEIVER_DRIVER_H_
#define CAN_RECEIVER_DRIVER_H_

//#define CAN_USED_ID		CAN_ID_STD
#define 	CAN_MSG_QUEUE_SIZE	64

#define	CAN_FILTERS_NUMBER		14	//TODO 14 czy 28?
#define	CAN_FRAMES_PER_FILTER	4
#define	CAN_MAX_NO_OF_FRAMES	(CAN_FILTERS_NUMBER * CAN_FRAMES_PER_FILTER)

#define	CAN_MAX_CHANNELS_PER_FRAME	8

typedef enum {
	CANReceiver_Status_OK = 0,
	CANReceiver_Status_Empty,
	CANReceiver_Status_TooManyFramesIDs,
	CANReceiver_Status_InitError,
	CANReceiver_Status_RunTimeError,
	CANReceiver_Status_NotSTDFrame,
	CANReceiver_Status_RTRFrame,
	CANReceiver_Status_Error
} CANReceiver_Status_TypeDef;

typedef struct {
	FIFOQueue					framesFIFO;
	CAN_HandleTypeDef*			phcan;
	volatile CanRxMsgTypeDef	rxHALMsg;
	MSTimerMiddleware_TypeDef*	pMSTimerMiddlewareHandler;
	ConfigDataManager_TypeDef*	pConfigManager;
	CANData_TypeDef 			pReceiverQueueBuffer [CAN_MSG_QUEUE_SIZE];
} CANReceiver_TypeDef;

CANReceiver_Status_TypeDef CANReceiver_init(CANReceiver_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager, CAN_HandleTypeDef* hcan, MSTimerMiddleware_TypeDef* pMSTimerMiddlewareHandler);
CANReceiver_Status_TypeDef CANReceiver_start(CANReceiver_TypeDef* pSelf);
CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANReceiver_TypeDef* pSelf, CANData_TypeDef* pRetMsg);

CANReceiver_Status_TypeDef CANReceiver_RxCpltCallback(CANReceiver_TypeDef* pSelf);
CANReceiver_Status_TypeDef CANReceiver_ErrorCallback(CANReceiver_TypeDef* pSelf);

#endif /* CAN_RECEIVER_DRIVER_H_ */

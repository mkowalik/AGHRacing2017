/*
 * can_receiver_driver.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "fifo_queue.h"
#include "can_data.h"
#include "config.h"

#ifndef CAN_RECEIVER_DRIVER_H_
#define CAN_RECEIVER_DRIVER_H_

//#define CAN_USED_ID		CAN_ID_STD
#define 	CAN_MSG_QUEUE_SIZE	64

typedef enum {
	CANReceiver_Status_OK = 0,
	CANReceiver_Status_Empty,
	CANReceiver_Status_TooManyFramesIDs,
	CANReceiver_Status_InitError,
	CANReceiver_Status_RunTimeError,
	CANReceiver_Status_Error
} CANReceiver_Status_TypeDef;

typedef struct {
	FIFOQueue					framesFIFO;
	CAN_HandleTypeDef*			phcan;
	volatile CanRxMsgTypeDef	rxHALMsg;
} CANReceiver_TypeDef;

/**
 * TODO Queue should be initialized
 *
 */
CANReceiver_Status_TypeDef CANReceiver_init(Config_TypeDef* pConfig, CAN_HandleTypeDef* hcan);
CANReceiver_Status_TypeDef CANReceiver_pullLastFrame(CANData_TypeDef* pRetMsg);

#endif /* CAN_RECEIVER_DRIVER_H_ */

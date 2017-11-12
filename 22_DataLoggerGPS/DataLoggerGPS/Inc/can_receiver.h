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
	CANReceiver_Status_Error
} CANReceiver_Status_TypeDef;

typedef struct {
	CANData_TypeDef	framesQueueTab[CAN_MSG_QUEUE_SIZE];
	FIFOQueue		framesFIFO;
} CANReceiver_TypeDef;

CANReceiver_Status_TypeDef CANReceiver_init(Config_TypeDef* pConfig);
CANReceiver_Status_TypeDef CANReceiver_getLastFrame(CANData_TypeDef* pRetMsg);

#endif /* CAN_RECEIVER_DRIVER_H_ */

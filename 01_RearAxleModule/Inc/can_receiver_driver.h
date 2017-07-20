/*
 * can_receiver_driver.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "stm32f0xx_hal.h"
#include "fifo_queue.h"

#ifndef CAN_RECEIVER_DRIVER_H_
#define CAN_RECEIVER_DRIVER_H_

#define CAN_USED_ID						CAN_ID_STD

#define FRONT_ACC_STOP_CHANNEL			0x100

void CAN_ReceiverDriver_init(volatile FIFOQueue* psgQueueArg);
void CAN_ReceiverDriver_receiveITHandler();
void CAN_ReceiverDriver_queueProcessedNotifier();

#endif /* CAN_RECEIVER_DRIVER_H_ */

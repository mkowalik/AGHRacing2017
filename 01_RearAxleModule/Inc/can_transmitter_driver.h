/*
 * can_receiver_driver.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "stm32f0xx_hal.h"
#include "fifo_queue.h"

#ifndef CAN_TRANSMITTER_DRIVER_H_
#define CAN_TRANSMITTER_DRIVER_H_

#define CAN_USED_ID						CAN_ID_STD

#define FRONT_ACC_STOP_CHANNEL			0x100

void CAN_TransmitterDriver_init(volatile FIFOQueue* psgQueueArg);
void CAN_TransmitterDriver_transmittITHandler();
void CAN_TransmitterDriver_queueProcessedNotifier();

#endif /* CAN_TRANSMITTER_DRIVER_H_ */

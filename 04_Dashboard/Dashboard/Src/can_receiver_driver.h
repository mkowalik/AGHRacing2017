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

#define RPM_CAN_CHANNEL					0x600
#define CLT_OIL_PRESSURE_CAN_CHANNEL	0x602
#define BATT_VOLTAGE_CAN_CHANNEL		0x604
#define FUEL_LEVEL_CAN_CHANNEL			0x400
#define GEAR_CAN_CHANNEL				0x080


void CAN_ReceiverDriver_init(volatile FIFOQueue* psgQueueArg);
void CAN_ReceiverDriver_receiveITHandler();
void CAN_ReceiverDriver_queueProcessedNotifier();
void CAN_ReceiverDriver_thread();

#endif /* CAN_RECEIVER_DRIVER_H_ */

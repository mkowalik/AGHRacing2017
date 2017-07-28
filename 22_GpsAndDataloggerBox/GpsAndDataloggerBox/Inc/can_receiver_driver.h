/*
 * can_receiver_driver.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */


#ifndef CAN_RECEIVER_DRIVER_H_
#define CAN_RECEIVER_DRIVER_H_

#include "stm32f1xx_hal.h"

#define BUFFER_SIZE		32

typedef enum {
	CANRDriverStatus_OK = 0,
	CANRDriverStatus_Empty
} CANRDriverStatus;

void CAN_ReceiverDriver_init();
CANRDriverStatus CAN_ReceiverDriver_getLastElement(CanRxMsgTypeDef* pRetMsg);

#endif /* CAN_RECEIVER_DRIVER_H_ */

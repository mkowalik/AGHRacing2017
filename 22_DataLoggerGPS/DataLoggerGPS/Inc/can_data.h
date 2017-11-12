/*
 * can_data.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef CAN_DATA_H_
#define CAN_DATA_H_

#include "precise_time_middleware.h"
#include "stdint.h"

typedef struct {
	uint16_t ID;
	uint8_t DLC;
	PreciseTime_TypeDef preciseTime;
	uint8_t Data[8];
} CANData_TypeDef;

#endif /* CAN_DATA_H_ */

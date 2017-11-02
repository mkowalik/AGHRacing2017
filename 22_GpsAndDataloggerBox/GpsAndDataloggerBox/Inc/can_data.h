/*
 * CANData.h
 *
 *  Created on: 02.08.2017
 *      Author: Kowalik
 */

#ifndef CAN_DATA_H_
#define CAN_DATA_H_

#define DATA_CHANNELS_NUMBER	2048 // = 2^11

typedef struct {
	uint32_t 		DLC;
	uint8_t 		Data[8];
	RTC_TimeTypeDef	arrivalTime;
} CANData_TypeDef ;

#endif /* CAN_DATA_H_ */

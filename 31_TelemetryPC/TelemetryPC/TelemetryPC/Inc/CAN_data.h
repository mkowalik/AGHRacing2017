/*
 * data.h
 *
 *  Created on: May 21, 2017
 *      Author: Wojciech
 */

#ifndef CAN_DATA_H_
#define CAN_DATA_H_

#include "stm32f0xx_hal.h"

typedef union{
	struct message{
		struct header{
			uint8_t reserved	: 5;
			uint8_t control 	: 1;
			uint8_t canFrames	: 2;
		} header;

		union data{
			struct can{
				uint8_t IDl;
				uint8_t IDh			: 3;
				uint8_t dataLength	: 4;
				uint8_t reserved	: 1;
				uint8_t data[8];
			} can[3];

			struct control{
				uint8_t	filterNum	: 6;
				uint8_t reserved	: 3;
				struct filter{
				    uint8_t IDl;
				    uint8_t IDh			: 3;
				    uint8_t onReq		: 1;
				    uint8_t onState     : 1;
					uint8_t reserved	: 3;
				} filter[14];
			} control;

		} data;
	} message;

	uint8_t payload[31];
} CanData_payload_t;

#endif /* CAN_DATA_H_ */

/*
 * fifo.h
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "stm32f0xx_hal.h"

#define FIFO_SIZE		100

typedef union{
	struct message{
		struct header{
			uint8_t reserved	: 3;
			uint8_t range		: 2;
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
				uint8_t	txPermission: 1;
				uint8_t	filterNum	: 6;
				uint8_t reserved	: 1;
				uint8_t speed;
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
} message_t;

typedef struct{
	message_t fifo[FIFO_SIZE];
	uint16_t firstEl;
	uint16_t lastEl;
	uint16_t elementsInFifo;
} fifo_t;

void Fifo_Init(fifo_t * fifo);
uint8_t Fifo_PullElement(fifo_t * fifo, message_t * Element);
uint8_t Fifo_PushElement(fifo_t * fifo, message_t * newElement);

#endif /* FIFO_H_ */

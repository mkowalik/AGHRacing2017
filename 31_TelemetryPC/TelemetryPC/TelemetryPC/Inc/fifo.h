/*
 * fifo.h
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "stm32f0xx_hal.h"

typedef enum{
  Fifo_OK       	= 0x00,
  Fifo_EMPTY		= 0x01,
  Fifo_FULL			= 0x02
} Fifo_StatusTypeDef;

typedef struct{
	int8_t *pcHead;
	int8_t *pcTail;
	int8_t *pcWriteTo;
	int8_t *pcReadFrom;

	uint32_t uxLength;
	uint32_t uxItemSize;
	volatile uint32_t uxMessagesWaiting;
} Fifo_Handle_t;

Fifo_Handle_t Fifo_Init(void * array, uint32_t elementSize, uint32_t fifoSize);
Fifo_StatusTypeDef Fifo_PullElement(Fifo_Handle_t * fifo, void * Element);
Fifo_StatusTypeDef Fifo_PushElement(Fifo_Handle_t * fifo, void * newElement);

#endif /* FIFO_H_ */

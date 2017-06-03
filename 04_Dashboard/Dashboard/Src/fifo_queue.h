/*
 * FIFOQueue.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include "stm32f0xx_hal.h"
#include "can.h"

#define ELEMENT_TYPE	CanRxMsgTypeDef

typedef enum {
	FIFOStatus_OK = 0,
	FIFOStatus_Full,
	FIFOStatus_Empty
} FIFOStatus;

typedef struct {
	ELEMENT_TYPE* pTabPtr;
	uint16_t size;
	uint16_t firstElement;
	uint16_t arterLastElement;
} FIFOQueue;

void FIFOQueue_init(FIFOQueue* self, ELEMENT_TYPE* pTabArg, uint16_t size);
FIFOStatus FIFOQueue_enqueue(FIFOQueue* self, ELEMENT_TYPE element);
FIFOStatus FIFOQueue_dequeue(FIFOQueue* self, ELEMENT_TYPE* pRetElement);
uint16_t FIFOQueue_elementsNumber(FIFOQueue* self);



#endif /* FIFOQUEUE_H_ */

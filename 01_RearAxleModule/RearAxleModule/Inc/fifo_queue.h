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
	volatile ELEMENT_TYPE* pTabPtr;
	uint16_t size;
	volatile uint16_t firstElement;
	volatile uint16_t afterLastElement;

	// Values below used only when sizeReduction function in progress and interrupt occours
	volatile uint8_t sizeReductionInPorgress;
	volatile uint16_t firstElementOld;
	volatile uint16_t afterLastElementOld;
} FIFOQueue;

void FIFOQueue_init(volatile FIFOQueue* self, volatile ELEMENT_TYPE* pTabArg, uint16_t size);
FIFOStatus FIFOQueue_enqueue(volatile FIFOQueue* self, ELEMENT_TYPE element);
FIFOStatus FIFOQueue_dequeue(volatile FIFOQueue* self, ELEMENT_TYPE* pRetElement);
uint16_t FIFOQueue_elementsNumber(volatile FIFOQueue* self);
uint8_t FIFOQueue_full(volatile FIFOQueue* self);



#endif /* FIFOQUEUE_H_ */

/*
 * FIFOQueue.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include "can.h"

typedef enum {
	FIFOStatus_OK = 0,
	FIFOStatus_Full,
	FIFOStatus_Empty,
	FIFOStatus_Error
} FIFOStatus;

typedef struct {
	uint8_t* pTabPtr;
	uint8_t elementSize;
	uint16_t queueLength;
	volatile uint16_t elementsNumber;
	volatile uint16_t headIndex;
	volatile uint16_t tailIndex;
} FIFOQueue;

void FIFOQueue_init(FIFOQueue* self, void* volatilepTabPtrArg, uint8_t elementSize, uint16_t size);
FIFOStatus FIFOQueue_enqueue(FIFOQueue* self, void* pElement);
FIFOStatus FIFOQueue_dequeue(FIFOQueue* self, void* pRetElement);
uint16_t FIFOQueue_elementsNumber(FIFOQueue* self);
uint8_t FIFOQueue_full(FIFOQueue* self);



#endif /* FIFOQUEUE_H_ */

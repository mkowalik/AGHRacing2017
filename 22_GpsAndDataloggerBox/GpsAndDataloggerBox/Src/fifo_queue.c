/*
 * FIFOQueue.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "fifo_queue.h"
#include "string.h"

static void FIFOQueue_pointersReduction(volatile FIFOQueue* self){

	if ((self->firstElement) >= (self->queueSize)) {

		self->sizeReductionInPorgress = 1;

		self->firstElement -= self->queueSize;
		self->afterLastElement -= self->queueSize;

		self->sizeReductionInPorgress = 0;

		self->firstElementOld = self->firstElement;
		self->afterLastElementOld = self->afterLastElement;

	}
}

void FIFOQueue_init(volatile FIFOQueue* self, volatile void* pTabArg, uint16_t queueSize, uint8_t elementSize){
	self->pTab = pTabArg;
	self->elementSize = elementSize;
	self->queueSize = queueSize;
	self->firstElement = 0;
	self->afterLastElement = 0;

	self->sizeReductionInPorgress = 0;
	self->firstElementOld = 0;
	self->afterLastElementOld = 0;
}

FIFOStatus FIFOQueue_enqueue(volatile FIFOQueue* pSelf, void* pElement){

	if (FIFOQueue_full(pSelf)) return FIFOStatus_Full;

	uint16_t index = ((pSelf->afterLastElement)++) % (pSelf->queueSize); // Notice, incrementing afterLastElementValue

	memcpy((void*) (pSelf->pTab + index * pSelf->elementSize), pElement,  pSelf->elementSize);

	return FIFOStatus_OK;

}

FIFOStatus FIFOQueue_dequeue(volatile FIFOQueue* pSelf, void* pRetElement){

	if (FIFOQueue_elementsNumber(pSelf) == 0) return FIFOStatus_Empty;

	uint16_t index = ((pSelf->firstElement)++) % (pSelf->queueSize); //Notice, incrementing firstElement value

	memcpy(pRetElement, (void*) (pSelf->pTab + index * pSelf->elementSize), pSelf->elementSize);

	FIFOQueue_pointersReduction(pSelf);

	return FIFOStatus_OK;
}

uint16_t FIFOQueue_elementsNumber(volatile FIFOQueue* pSelf){

	if (pSelf->sizeReductionInPorgress)
		return pSelf->afterLastElementOld - pSelf->firstElementOld;
	else
		return pSelf->afterLastElement - pSelf->firstElement;

}

uint8_t FIFOQueue_full(volatile FIFOQueue* pSelf){

	return ((FIFOQueue_elementsNumber(pSelf)) >= (pSelf->queueSize));

}


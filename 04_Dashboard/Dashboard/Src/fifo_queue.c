/*
 * FIFOQueue.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "fifo_queue.h"

static void FIFOQueue_sizeReduction(volatile FIFOQueue* self){

	if ((self->firstElement) >= (self->size)) {

		self->sizeReductionInPorgress = 1;

		self->firstElement -= self->size;
		self->afterLastElement -= self->size;

		self->sizeReductionInPorgress = 0;

		self->firstElementOld -= self->size;
		self->afterLastElementOld -= self->size;

	}
}

void FIFOQueue_init(volatile FIFOQueue* self, volatile ELEMENT_TYPE* pTabPtrArg, uint16_t size){
	self->pTabPtr = pTabPtrArg;
	self->size = size;
	self->firstElement = 0;
	self->afterLastElement = 0;

	self->sizeReductionInPorgress = 0;
	self->firstElementOld = 0;
	delf->afterLastElementOld = 0;
}

FIFOStatus FIFOQueue_enqueue(volatile FIFOQueue* pSelf, ELEMENT_TYPE element){

	if (FIFOQueue_full(pSelf)) return FIFOStatus_Full;

	uint16_t index = ((pSelf->afterLastElement)++) % (pSelf->size); // Notice, incrementing afterLastElementValue

	pSelf->pTabPtr[index] = element;

	return FIFOStatus_OK;

}

FIFOStatus FIFOQueue_dequeue(volatile FIFOQueue* self, ELEMENT_TYPE* pRetElement){

	if (FIFOQueue_elementsNumber(self) == 0) return FIFOStatus_Empty;

	ELEMENT_TYPE ret = self->pTabPtr[((self->firstElement)++) % (self->size)];	//Notice, incrementing firstElement value

	FIFOQueue_sizeReduction(self);

	(*pRetElement) = ret;

	return FIFOStatus_OK;
}

uint16_t FIFOQueue_elementsNumber(volatile FIFOQueue* self){

	if (self->sizeReductionInPorgress)
		return self->afterLastElementOld - self->firstElementOld;
	else
		return self->afterLastElement - self->firstElement;

}

uint8_t FIFOQueue_full(volatile FIFOQueue* self){

	return ((FIFOQueue_elementsNumber(self)) >= (self->size));

}

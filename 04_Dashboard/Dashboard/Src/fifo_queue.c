/*
 * FIFOQueue.c
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#include "fifo_queue.h"

void FIFOQueue_init(FIFOQueue* self, ELEMENT_TYPE* pTabPtrArg, uint16_t size){
	self->pTabPtr = pTabPtrArg;
	self->size = size;
	self->firstElement = 0;
	self->arterLastElement = 0;
}

FIFOStatus FIFOQueue_enqueue(FIFOQueue* self, ELEMENT_TYPE element){

	//TODO prawdopodobnie sie wysypie jak bedzie uzywane w przerwaniu, a bedzie

	if ((self->arterLastElement) - (self->firstElement) >= self->size) return FIFOStatus_Full;

	uint16_t index = ((self->arterLastElement)++) % (self->size); // Notice, incrementing afgterLastElementValue

	self->pTabPtr[index] = element;

	return FIFOStatus_OK;

}

FIFOStatus FIFOQueue_dequeue(FIFOQueue* self, ELEMENT_TYPE* pRetElement){

	if (FIFOQueue_elementsNumber(self) == 0) return FIFOStatus_Empty;

	ELEMENT_TYPE ret = self->pTabPtr[((self->firstElement)++) % (self->size)];	//Notice, incrementing firstElement value

	if ((self->firstElement) >= (self->size)) {
		self->firstElement -= self->size;
		self->arterLastElement -= self->size;
	}

	(*pRetElement) = ret;

	return FIFOStatus_OK;
}

uint16_t FIFOQueue_elementsNumber(FIFOQueue* self){

	return self->arterLastElement - self->firstElement;

}

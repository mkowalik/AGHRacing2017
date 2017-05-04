/*
 * fifo.c
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */

#include <fifo.h>

void Fifo_Init(fifo_t * fifo){
	fifo->elementsInFifo = 0;
	fifo->firstEl = 1;
	fifo->lastEl = 0;
}

uint8_t Fifo_PullElement(fifo_t * fifo, message_t * Element){

	if(fifo->elementsInFifo == 0){
		return 0;
	}

	*Element = fifo->fifo[fifo->firstEl];
	fifo->firstEl++;
	if(fifo->firstEl == FIFO_SIZE){
		fifo->firstEl = 0;
	}
	fifo->elementsInFifo--;
	return 1;
}

uint8_t Fifo_PushElement(fifo_t * fifo, message_t * newElement){

	if(fifo->elementsInFifo == FIFO_SIZE){
		return 0;
	}

	fifo->lastEl++;
	if(fifo->lastEl == FIFO_SIZE){
		fifo->lastEl = 0;
	}
	fifo->fifo[fifo->lastEl] = *newElement;
	fifo->elementsInFifo++;
	return 1;
}

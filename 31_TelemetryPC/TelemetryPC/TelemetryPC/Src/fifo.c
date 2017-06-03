/*
 * fifo.c
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */

#include "fifo.h"
#include <string.h>
#include <stdio.h>

Fifo_Handle_t Fifo_Init(void * array, uint32_t elementSize, uint32_t fifoSize){

	Fifo_Handle_t handle;

	handle.uxItemSize = elementSize;
	handle.uxMessagesWaiting = 0;
	handle.uxLength = fifoSize;
	handle.pcHead = ( ( int8_t * ) array );
	handle.pcTail = handle.pcHead + (handle.uxLength * handle.uxItemSize);
	handle.pcWriteTo = handle.pcHead;
	handle.pcReadFrom = handle.pcHead + ( ( handle.uxLength - ( uint32_t ) 1U ) * handle.uxItemSize );

	return handle;
}

Fifo_StatusTypeDef Fifo_PullElement(Fifo_Handle_t * fifo, void * Element){

	if(fifo->uxMessagesWaiting == 0){
		return Fifo_EMPTY;
	}

	fifo->pcReadFrom += fifo->uxItemSize;
	if(fifo->pcReadFrom >= fifo->pcTail){
		fifo->pcReadFrom = fifo->pcHead;
	}

	(void) memcpy( (void *) Element, (void *) fifo->pcReadFrom, (size_t) fifo->uxItemSize);

	fifo->uxMessagesWaiting--;
	return Fifo_OK;
}

Fifo_StatusTypeDef Fifo_PushElement(Fifo_Handle_t * fifo, void * newElement){

	if(fifo->uxMessagesWaiting == fifo->uxLength){
		return Fifo_FULL;
	}

	( void ) memcpy( ( void * ) fifo->pcWriteTo, newElement, ( size_t ) fifo->uxItemSize );

	fifo->pcWriteTo += fifo->uxItemSize;
	if( fifo->pcWriteTo >= fifo->pcTail ){
		fifo->pcWriteTo = fifo->pcHead;
	}

	fifo->uxMessagesWaiting++;
	return Fifo_OK;
}
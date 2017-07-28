/*
 * FIFOQueue.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include "stm32f1xx_hal.h"

typedef struct {
	volatile void* pTab;
	uint8_t elementSize;
	uint16_t queueSize;
	volatile uint16_t firstElement;
	volatile uint16_t afterLastElement;

	// Values below used only when sizeReduction function in progress and interrupt occours
	volatile uint8_t sizeReductionInPorgress;
	volatile uint16_t firstElementOld;
	volatile uint16_t afterLastElementOld;
} FIFOQueue;

typedef enum {
	FIFOStatus_OK = 0,
	FIFOStatus_Full,
	FIFOStatus_Empty
} FIFOStatus;

void FIFOQueue_init(volatile FIFOQueue* pSelf, volatile void* pTab, uint16_t queueSize, uint8_t elementSize);
FIFOStatus FIFOQueue_enqueue(volatile FIFOQueue* pSelf, void* pElement);
FIFOStatus FIFOQueue_dequeue(volatile FIFOQueue* pSelf, void* pRetElement);
uint16_t FIFOQueue_elementsNumber(volatile FIFOQueue* pSelf);
uint8_t FIFOQueue_full(volatile FIFOQueue* pSelf);

#endif /* FIFOQUEUE_H_ */

/*
 * fifo.h
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */

#ifndef FIFO_H_
#define FIFO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

typedef enum{
  FIFO_OK       = 0x00,
  FIFO_EMPTY    = 0x01,
  FIFO_FULL     = 0x02
} Fifo_StatusTypeDef;

typedef struct{
    volatile int8_t *volatile pcHead;
    volatile int8_t *volatile pcTail;
    volatile int8_t *volatile pcWriteTo;
    volatile int8_t *volatile pcReadFrom;

    uint32_t uxLength;
    uint32_t uxItemSize;
    volatile uint32_t uxMessagesWaiting;
} Fifo_Handle_t;

Fifo_Handle_t Fifo_Init(uint32_t elementSize, uint32_t fifoSize);
Fifo_StatusTypeDef Fifo_PullElement(Fifo_Handle_t * fifo, void * Element);
Fifo_StatusTypeDef Fifo_PushElement(Fifo_Handle_t * fifo, void * newElement);
Fifo_StatusTypeDef Fifo_PushElementToFront(Fifo_Handle_t * fifo, void * newElement);

#ifdef __cplusplus
}
#endif
#endif /* FIFO_H_ */

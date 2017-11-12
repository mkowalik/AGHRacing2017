/*
 * memory_buffer_middleware.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef MEMORY_BUFFER_MIDDLEWARE_H_
#define MEMORY_BUFFER_MIDDLEWARE_H_

#include "can_data.h"
#include "fifo_queue.h"

#define MEMORY_MSG_QUEUE_SIZE	64

typedef struct {
	CANData_TypeDef	framesQueueTab[MEMORY_MSG_QUEUE_SIZE];
	FIFOQueue		framesFIFO;
} MemoryBufferMiddleware_TypeDef;

typedef enum {
	MemoryBufferMiddleware_OK = 0,
	MemoryBufferMiddleware_MemoryFull,
	MemoryBufferMiddleware_NoFileOpened,
	MemoryBufferMiddleware_NoMemoryDevice,
	MemoryBufferMiddleware_Error
} MemoryBufferMiddleware_Status_TypeDef;

MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_init();
MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_openFile(char* filename);
MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_closeFile();
MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_writeData(CANData_TypeDef* data);

#endif /* MEMORY_BUFFER_MIDDLEWARE_H_ */

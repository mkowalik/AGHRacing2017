/*
 * meory_buffer_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include "memory_buffer_middleware.h"
#include "precise_time_middleware.h"

MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_init(){

}

MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_openFile(char* pFilename){

}

MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_closeFile(){

}

MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_writeData(CANData_TypeDef* pData){

}

static MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_bufferFlush(){

}

static MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_saveHeader(RTC_DateTypeDef date, PreciseTime_TypeDef preciseTime){

}

static MemoryBufferMiddleware_Status_TypeDef MemoryBufferMiddleware_saveToBuffer(CANData_TypeDef* pData){

}

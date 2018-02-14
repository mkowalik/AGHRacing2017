/*
 * meory_buffer_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include "data_saver_buffer.h"
#include "precise_time_middleware.h"
#include "stdint.h"

DataSaverBuffer_Status_TypeDef DataSaverBuffer_init(DataSaverBuffer_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfig){

	pSelf->state = DataSaverBuffer_State_UnInitialized;

	if ((pSelf->state != DataSaverBuffer_State_UnInitialized) != 0){
		return DataSaverBuffer_Status_OK;
	}

	pSelf->pConfig = pConfig;

	FileSystemMiddleware_Status_TypeDef result1 = FileSystemMiddleware_init();

	if (result1 != FileSystemMiddleware_Status_OK){
		return DataSaverBuffer_Status_NoMemoryDeviceError;
	}

	FIFOStatus result2 = FIFOQueue_init(&(pSelf->framesFIFO), pSelf->framesQueueTab, sizeof(CANData_TypeDef), MEMORY_MSG_QUEUE_SIZE);

	if (result2 != FIFOStatus_OK){
		return DataSaverBuffer_Status_Error;
	}

	pSelf->state = DataSaverBuffer_State_Initialized;

	return DataSaverBuffer_Status_OK;

}

DataSaverBuffer_Status_TypeDef DataSaverBuffer_openFile(DataSaverBuffer_TypeDef* pSelf, char* pFilename){

	if ((pSelf->state & DataSaverBuffer_State_Initialized) == 0){
		return DataSaverBuffer_Status_UnInitializedError;
	}

	if ((pSelf->state & DataSaverBuffer_State_OpenedFile) != 0){
		return DataSaverBuffer_Status_AlreadyOpenedFileError;
	}

	FileSystemMiddleware_Status_TypeDef result = FileSystemMiddleware_open(&(pSelf->sDataFile), pFilename);

	if (result != FileSystemMiddleware_Status_OK){
		return DataSaverBuffer_Status_FileNotOpenedError;
	}

	pSelf->state = DataSaverBuffer_State_OpenedFile;

	return DataSaverBuffer_Status_OK;

}

DataSaverBuffer_Status_TypeDef DataSaverBuffer_closeFile(DataSaverBuffer_TypeDef* pSelf){

	if (pSelf->state != DataSaverBuffer_State_Initialized){
		return DataSaverBuffer_Status_UnInitializedError;
	}

	if (pSelf->state != DataSaverBuffer_State_OpenedFile){
		return DataSaverBuffer_Status_FileNotOpenedError;
	}

	FileSystemMiddleware_Status_TypeDef result = FileSystemMiddleware_close(&(pSelf->sDataFile));

	if (result != FileSystemMiddleware_Status_OK){
		return DataSaverBuffer_Status_Error;
	}

	pSelf->state = DataSaverBuffer_State_Initialized;

	return DataSaverBuffer_Status_OK;

}

DataSaverBuffer_Status_TypeDef DataSaverBuffer_writeData(DataSaverBuffer_TypeDef* pSelf, CANData_TypeDef* pData){

	if ((pSelf->state & DataSaverBuffer_State_Initialized) == 0){
		return DataSaverBuffer_Status_UnInitializedError;
	}

	if ((pSelf->state & DataSaverBuffer_State_OpenedFile) == 0){
		return DataSaverBuffer_Status_FileNotOpenedError;
	}

	uint8_t buffer[16];

	buffer[0] = (pData->preciseTime.unixTime >> 3) & 0xFF;
	buffer[1] = (pData->preciseTime.unixTime >> 2) & 0xFF;
	buffer[2] = (pData->preciseTime.unixTime >> 1) & 0xFF;
	buffer[3] = (pData->preciseTime.unixTime >> 0) & 0xFF;

	buffer[4] = (pData->preciseTime.miliseconds >> 1) & 0xFF;
	buffer[5] = (pData->preciseTime.miliseconds >> 0) & 0xFF;

	buffer[6] = (pData->ID >> 1) & 0xFF;
	buffer[7] = (pData->ID >> 0) & 0xFF;

	for (uint8_t i=0; i<pData->DLC; i++){
		buffer[8+i] = pData->Data[i];
	}

	uint32_t	bytesToWrite = 8 + pData->DLC;
	uint32_t	bytesWritten = 0;

	FileSystemMiddleware_Status_TypeDef status = FileSystemMiddleware_writeData(&(pSelf->sDataFile), buffer, bytesToWrite, &bytesWritten);

	if ((bytesToWrite != bytesWritten) || (status != FileSystemMiddleware_Status_OK)){
		return DataSaverBuffer_Status_Error;
	}

	return DataSaverBuffer_Status_OK;

}

static DataSaverBuffer_Status_TypeDef MemoryBufferMiddleware_saveHeader(DataSaverBuffer_TypeDef* pSelf, PreciseTime_TypeDef preciseTime){

	uint8_t buffer[16];
	uint32_t bytesWritten, bytesRead;

	buffer[0] = (LOG_FILE_VERSION << 1) & 0xFF;
	buffer[1] = (LOG_FILE_VERSION << 0) & 0xFF;

	buffer[2] = (LOG_FILE_SUBVERSION << 1) & 0xFF;
	buffer[3] = (LOG_FILE_SUBVERSION << 0) & 0xFF;

	FileSystemMiddleware_Status_TypeDef status = FileSystemMiddleware_writeData(&(pSelf->sDataFile), buffer, 4, &bytesWritten);

	if ((bytesWritten != 4) ||(status != FileSystemMiddleware_Status_OK)){
		return DataSaverBuffer_Status_Error;
	}

	do {

		status = FileSystemMiddleware_readData(&(pSelf->pConfig->sConfigFileHandler), buffer, 16, &bytesRead);
		if (status != FileSystemMiddleware_Status_OK){
			return DataSaverBuffer_Status_Error;
		}

		status = FileSystemMiddleware_writeData(&(pSelf->sDataFile), buffer, bytesRead, &bytesWritten);
		if (status != FileSystemMiddleware_Status_OK){
			return DataSaverBuffer_Status_Error;
		}
	} while (bytesRead == 16);

	return DataSaverBuffer_Status_OK;

}

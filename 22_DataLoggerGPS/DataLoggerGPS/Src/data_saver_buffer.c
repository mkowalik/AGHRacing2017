/*
 * meory_buffer_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include "data_saver_buffer.h"
#include "stdint.h"

static DataSaverBuffer_Status_TypeDef DataSaverBuffer_saveHeader(DataSaverBuffer_TypeDef* pSelf, DateTime_TypeDef dateTime);

DataSaverBuffer_Status_TypeDef DataSaverBuffer_init(DataSaverBuffer_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfig, RTCMiddleware_TypeDef* pRTCMiddlewareHandler){

	if (pSelf->state != DataSaverBuffer_State_UnInitialized){
		return DataSaverBuffer_Status_Error;
	}

	pSelf->pConfigHandler = pConfig;
	pSelf->pRTCMiddlewareHandler = pRTCMiddlewareHandler;

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

	FileSystemMiddleware_Status_TypeDef status1 = FileSystemMiddleware_open(&(pSelf->sDataFile), pFilename);

	if (status1 != FileSystemMiddleware_Status_OK){
		return DataSaverBuffer_Status_FileNotOpenedError;
	}

	pSelf->state = DataSaverBuffer_State_OpenedFile;

	DateTime_TypeDef dateTime;

	RTCMiddleware_Status_TypeDef status2 = RTCMiddleware_getDateAndTime(pSelf->pRTCMiddlewareHandler, &dateTime);

	if (status2 != RTCMiddleware_Status_OK){
		return DataSaverBuffer_Status_Error;
	}

	DataSaverBuffer_saveHeader(pSelf, dateTime);

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

	buffer[0] = (pData->msTime >> 0 ) & 0xFF;
	buffer[1] = (pData->msTime >> 8 ) & 0xFF;
	buffer[2] = (pData->msTime >> 16) & 0xFF;
	buffer[3] = (pData->msTime >> 24) & 0xFF;

	buffer[4] = (pData->ID >> 0) & 0xFF;
	buffer[5] = (pData->ID >> 8) & 0xFF;

	for (uint8_t i=0; i<pData->DLC; i++){
		buffer[6+i] = pData->Data[i];
	}

	uint32_t	bytesToWrite = 6 + pData->DLC;
	uint32_t	bytesWritten = 0;

	FileSystemMiddleware_Status_TypeDef status = FileSystemMiddleware_writeBinaryData(&(pSelf->sDataFile), buffer, bytesToWrite, &bytesWritten);

	if ((bytesToWrite != bytesWritten) || (status != FileSystemMiddleware_Status_OK)){
		return DataSaverBuffer_Status_Error;
	}

	return DataSaverBuffer_Status_OK;

}

static DataSaverBuffer_Status_TypeDef DataSaverBuffer_saveHeader(DataSaverBuffer_TypeDef* pSelf, DateTime_TypeDef dateTime){

	uint8_t buffer[16];
	uint32_t bytesWritten, bytesRead;

	buffer[0] = (LOG_FILE_VERSION >> 0) & 0xFF;
	buffer[1] = (LOG_FILE_VERSION >> 8) & 0xFF;

	buffer[2] = (LOG_FILE_SUBVERSION >> 0) & 0xFF;
	buffer[3] = (LOG_FILE_SUBVERSION >> 8) & 0xFF;

	FileSystemMiddleware_Status_TypeDef status1 = FileSystemMiddleware_writeBinaryData(&(pSelf->sDataFile), buffer, 4, &bytesWritten);

	if ((bytesWritten != 4) ||(status1 != FileSystemMiddleware_Status_OK)){
		return DataSaverBuffer_Status_Error;
	}

	do {
		status1 = FileSystemMiddleware_readData(&(pSelf->pConfigHandler->sConfigFileHandler), buffer, 16, &bytesRead);
		if (status1 != FileSystemMiddleware_Status_OK){
			return DataSaverBuffer_Status_Error;
		}

		status1 = FileSystemMiddleware_writeBinaryData(&(pSelf->sDataFile), buffer, bytesRead, &bytesWritten);
		if (status1 != FileSystemMiddleware_Status_OK){
			return DataSaverBuffer_Status_Error;
		}

	} while (bytesRead == 16);

	buffer[0] = (dateTime.year >> 0) & 0xFF;
	buffer[1] = (dateTime.year >> 8) & 0xFF;

	buffer[2] = dateTime.month;
	buffer[3] = dateTime.day;
	buffer[4] = dateTime.hour;
	buffer[5] = dateTime.minute;
	buffer[6] = dateTime.second;

	status1 = FileSystemMiddleware_writeBinaryData(&(pSelf->sDataFile), buffer, 7, &bytesWritten);

	if ((bytesWritten != 6) || (status1 != FileSystemMiddleware_Status_OK)){
		return DataSaverBuffer_Status_Error;
	}

	return DataSaverBuffer_Status_OK;

}

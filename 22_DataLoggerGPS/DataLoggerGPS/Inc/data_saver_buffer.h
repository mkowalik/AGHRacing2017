/*
 * data_buffer_middleware.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef DATA_SAVER_BUFFER_H_
#define DATA_SAVER_BUFFER_H_

#include "can_data.h"
#include "fifo_queue.h"
#include "config.h"

#define MEMORY_MSG_QUEUE_SIZE	64

#define	LOG_FILE_VERSION	1
#define	LOG_FILE_SUBVERSION	0

typedef enum {
	DataSaverBuffer_State_UnInitialized = 0x01,
	DataSaverBuffer_State_Initialized 	= 0x01,
	DataSaverBuffer_State_OpenedFile 	= 0x02
} DataSaverBuffer_State_TypeDef;

typedef struct {
	CANData_TypeDef						framesQueueTab[MEMORY_MSG_QUEUE_SIZE];
	FIFOQueue							framesFIFO;
	FileSystemMiddleware_File_TypeDef	sDataFile;
	DataSaverBuffer_State_TypeDef		state;
	ConfigDataManager_TypeDef*			pConfig;
} DataSaverBuffer_TypeDef;

typedef enum {
	DataSaverBuffer_Status_OK = 0,
	DataSaverBuffer_Status_MemoryFullError,
	DataSaverBuffer_Status_FileNotOpenedError,
	DataSaverBuffer_Status_NoMemoryDeviceError,
	DataSaverBuffer_Status_UnInitializedError,
	DataSaverBuffer_Status_AlreadyOpenedFileError,
	DataSaverBuffer_Status_Error
} DataSaverBuffer_Status_TypeDef;

DataSaverBuffer_Status_TypeDef DataSaverBuffer_init(DataSaverBuffer_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfig);
DataSaverBuffer_Status_TypeDef DataSaverBuffer_openFile(DataSaverBuffer_TypeDef* pSelf, char* pFilename);
DataSaverBuffer_Status_TypeDef DataSaverBuffer_closeFile(DataSaverBuffer_TypeDef* pSelf);
DataSaverBuffer_Status_TypeDef DataSaverBuffer_writeData(DataSaverBuffer_TypeDef* pSelf, CANData_TypeDef* pData);

#endif /* DATA_SAVER_BUFFER_H_ */

/*
 * file_middleware.c
 *
 *  Created on: 27.01.2018
 *      Author: Kowalik
 */

#include "file_middleware.h"
#include "string.h"

static volatile FileSystemMiddleware_TypeDef sFileSystem;

static FileSystemMiddleware_Status_TypeDef remapResult(FRESULT res){

	switch (res){
	case FR_OK: 					return FileSystemMiddleware_Status_OK;
	case FR_DISK_ERR:				return FileSystemMiddleware_Status_DISK_ERR;
	case FR_INT_ERR:				return FileSystemMiddleware_Status_INT_ERR;
	case FR_NOT_READY:				return FileSystemMiddleware_Status_NOT_READY;
	case FR_NO_FILE:				return FileSystemMiddleware_Status_NO_FILE;
	case FR_NO_PATH:				return FileSystemMiddleware_Status_NO_PATH;
	case FR_INVALID_NAME:			return FileSystemMiddleware_Status_INVALID_NAME;
	case FR_DENIED:					return FileSystemMiddleware_Status_DENIED;
	case FR_EXIST:					return FileSystemMiddleware_Status_EXIST;
	case FR_INVALID_OBJECT:			return FileSystemMiddleware_Status_INVALID_OBJECT;
	case FR_WRITE_PROTECTED:		return FileSystemMiddleware_Status_WRITE_PROTECTED;
	case FR_INVALID_DRIVE:			return FileSystemMiddleware_Status_INVALID_DRIVE;
	case FR_NOT_ENABLED:			return FileSystemMiddleware_Status_NOT_ENABLED;
	case FR_NO_FILESYSTEM:			return FileSystemMiddleware_Status_NO_FILESYSTEM;
	case FR_MKFS_ABORTED:			return FileSystemMiddleware_Status_MKFS_ABORTED;
	case FR_TIMEOUT:				return FileSystemMiddleware_Status_TIMEOUT;
	case FR_LOCKED:					return FileSystemMiddleware_Status_LOCKED;
	case FR_NOT_ENOUGH_CORE:		return FileSystemMiddleware_Status_NOT_ENOUGH_CORE;
	case FR_TOO_MANY_OPEN_FILES:	return FileSystemMiddleware_Status_TOO_MANY_OPEN_FILES;
	case FR_INVALID_PARAMETER:		return FileSystemMiddleware_Status_INVALID_PARAMETER;
	default:						return FileSystemMiddleware_Status_DISK_ERR;
	}

}

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_init(){
	if (sFileSystem.bInitialized == 1){
		return FileSystemMiddleware_Status_OK;
	}

	FRESULT res = f_mount( (FATFS*) &(sFileSystem.sFatFS), "", FILESYSTEM_MOUNT_IMMEDIATELY);
	if (res == FR_OK){
		sFileSystem.bInitialized = 1;
	} else {
		sFileSystem.bInitialized = 0;
	}
	return remapResult(res);
}

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_open(FileSystemMiddleware_File_TypeDef* pFile, char* pFilename){

	pFile->pFileSystem = &sFileSystem;

	FRESULT res = f_open( (FIL*) &(pFile->sFile), pFilename, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	return remapResult(res);
}

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_writeData(FileSystemMiddleware_File_TypeDef* pFile, const void* pBuffer, uint32_t uiBytesToWrite, uint32_t* pBytesWritten){
	FRESULT res = f_write ( (FIL*) &(pFile->sFile), pBuffer, uiBytesToWrite, (UINT*) pBytesWritten);
	return remapResult(res);
}
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_readData(FileSystemMiddleware_File_TypeDef* pFile, void* pBuffer, uint32_t uiBytesToRead, uint32_t* pBytesRead){
	FRESULT res = f_read( (FIL*) &(pFile->sFile), pBuffer, uiBytesToRead, (UINT*) pBytesRead);
	return remapResult(res);
}

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_putString(FileSystemMiddleware_File_TypeDef* pFile, const char* pBuffer){
	if (f_puts (pBuffer, (FIL*) &(pFile->sFile)) != strlen(pBuffer)){
		return FileSystemMiddleware_Status_DISK_ERR;
	}
	return FileSystemMiddleware_Status_OK;
}

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_close(FileSystemMiddleware_File_TypeDef* pFile){
	FRESULT res = f_close( (FIL*) &(pFile->sFile));
	return remapResult(res);
}


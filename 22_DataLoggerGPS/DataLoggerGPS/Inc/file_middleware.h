/*
 * file_middleware.h
 *
 *  Created on: 27.01.2018
 *      Author: Kowalik
 */

#ifndef FILE_MIDDLEWARE_H_
#define FILE_MIDDLEWARE_H_

#include "ff.h"

#define	FILESYSTEM_MOUNT_IMMEDIATELY	1

typedef enum {
	FileSystemMiddleware_Status_OK = 0,					/* (0) Succeeded */
	FileSystemMiddleware_Status_DISK_ERR,				/* (1) A hard error occurred in the low level disk I/O layer */
	FileSystemMiddleware_Status_INT_ERR,				/* (2) Assertion failed */
	FileSystemMiddleware_Status_NOT_READY,				/* (3) The physical drive cannot work */
	FileSystemMiddleware_Status_NO_FILE,				/* (4) Could not find the file */
	FileSystemMiddleware_Status_NO_PATH,				/* (5) Could not find the path */
	FileSystemMiddleware_Status_INVALID_NAME,			/* (6) The path name format is invalid */
	FileSystemMiddleware_Status_DENIED,					/* (7) Access denied due to prohibited access or directory full */
	FileSystemMiddleware_Status_EXIST,					/* (8) Access denied due to prohibited access */
	FileSystemMiddleware_Status_INVALID_OBJECT,			/* (9) The file/directory object is invalid */
	FileSystemMiddleware_Status_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FileSystemMiddleware_Status_INVALID_DRIVE,			/* (11) The logical drive number is invalid */
	FileSystemMiddleware_Status_NOT_ENABLED,			/* (12) The volume has no work area */
	FileSystemMiddleware_Status_NO_FILESYSTEM,			/* (13) There is no valid FAT volume */
	FileSystemMiddleware_Status_MKFS_ABORTED,			/* (14) The f_mkfs() aborted due to any parameter error */
	FileSystemMiddleware_Status_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FileSystemMiddleware_Status_LOCKED,					/* (16) The operation is rejected according to the file sharing policy */
	FileSystemMiddleware_Status_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FileSystemMiddleware_Status_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FileSystemMiddleware_Status_INVALID_PARAMETER		/* (19) Given parameter is invalid */
} FileSystemMiddleware_Status_TypeDef;

typedef struct {
	volatile FATFS		sFatFS;
	volatile uint8_t	bInitialized;
} FileSystemMiddleware_TypeDef;

typedef struct {
	volatile FileSystemMiddleware_TypeDef* volatile	pFileSystem;
	volatile FIL									sFile;
} FileSystemMiddleware_File_TypeDef;

FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_init();
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_open(FileSystemMiddleware_File_TypeDef* pFile, char* pName);
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_putString(FileSystemMiddleware_File_TypeDef* pFile, const char* pBuffer);
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_close(FileSystemMiddleware_File_TypeDef* pFile);
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_writeBinaryData(FileSystemMiddleware_File_TypeDef* pFile, const void* pBuffer, uint32_t uiBytesToWrite, uint32_t* pBytesWritten);
FileSystemMiddleware_Status_TypeDef FileSystemMiddleware_readData(FileSystemMiddleware_File_TypeDef* pFile, void* pBuffer, uint32_t uiBytesToRead, uint32_t* pBytesRead);

#endif /* FILE_MIDDLEWARE_H_ */

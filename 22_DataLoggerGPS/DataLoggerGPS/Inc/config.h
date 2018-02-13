/*
 * config.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "can_data.h"
#include "can_receiver.h"
#include "file_middleware.h"

#define	CONFIG_SIGNED_TYPE_flag		(1<<0)
#define	CONFIG_16_BIT_TYPE_flag		(1<<1)
#define	CONFIG_ON_OFF_TYPE_flag		(1<<2)
#define	CONFIG_FLAG_TYPE_flag		(1<<3)
#define	CONFIG_CUSTOM_TYPE_flag		(1<<4)

#define	CONFIG_FILENAME		"logger.aghconf"

#define	CONFIG_FILE_USED_VERSION		1
#define	CONIG_FILE_USED_SUBVERSION	1

#define CONFIG_MAX_ID			2048

#define	CONFIG_MAX_DLC_VALUE	8

typedef enum {
	UNSIGNED_8BIT 	= (0x00),
	SIGNED_8BIT 	= (CONFIG_SIGNED_TYPE_flag),
	UNSIGNED_16BIT 	= (CONFIG_16_BIT_TYPE_flag),
	SIGNED_16BIT 	= (CONFIG_SIGNED_TYPE_flag | CONFIG_16_BIT_TYPE_flag),
	ON_OFF_8BIT 	= (CONFIG_ON_OFF_TYPE_flag),	//0x00 - OFF, 0xFF - ON
	FLAGS_8BIT 		= (CONFIG_FLAG_TYPE_flag),		//look into documentation for bits meaning definition
	FLAGS_16BIT 	= (CONFIG_FLAG_TYPE_flag | CONFIG_16_BIT_TYPE_flag),	//look into documentation for bits meaning definition
	CUSTOM_8BIT 	= (CONFIG_CUSTOM_TYPE_flag),	//look into documentation
	CUSTOM_16BIT 	= (CONFIG_CUSTOM_TYPE_flag | CONFIG_16_BIT_TYPE_flag)	//look into documentation
} ValueType_TypeDef;

typedef struct {
	ValueType_TypeDef 	valueType;
	uint16_t 			multiplier;
	uint16_t 			divider;
	uint16_t 			offset;
} ConfigChannel_TypeDef;

typedef struct {
	uint16_t 				ID;
	uint8_t 				DLC;
	ConfigChannel_TypeDef 	channels[CAN_MAX_CHANNELS_PER_FRAME];
} ConfigFrame_TypeDef;

typedef struct {
	uint16_t 				version;
	uint16_t 				subversion;
	uint16_t 				num_of_frames;
	ConfigFrame_TypeDef 	frames[CAN_MAX_NO_OF_FRAMES];
	ConfigFrame_TypeDef* 	framesByID[CONFIG_MAX_ID];
} Config_TypeDef;


typedef enum {
	ConfigDataManager_Status_OK = 0,
	ConfigDataManager_Status_NoConfigFileError,
	ConfigDataManager_Status_NoSuchChannelError,
	ConfigDataManager_Status_ConfigFileWrongVersionError,
	ConfigDataManager_Status_ConfigFileDataError,
	ConfigDataManager_Status_WrongOffsetError,
	ConfigDataManager_Status_WrongID,
	ConfigDataManager_Status_WrongDLC,
	ConfigDataManager_Status_NotInitialised,
	ConfigDataManager_Status_Error
} ConfigDataManager_Status_TypeDef;

typedef struct {
	uint8_t								initialised;
	Config_TypeDef						sConfig;
	FileSystemMiddleware_File_TypeDef	sConfigFileHandler;
} ConfigDataManager_TypeDef;

#define	CONFIG_FILE_NAME	"config.aghconf"

ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf);
ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig);
ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten);
ConfigDataManager_Status_TypeDef ConfigDataManager_checkCorrectnessData(ConfigDataManager_TypeDef* pSelf, CANData_TypeDef* pData);
ConfigDataManager_Status_TypeDef ConfigDataManager_findChannel(ConfigDataManager_TypeDef* pSelf, uint16_t ID, uint8_t offset, ConfigChannel_TypeDef* pRetChannel);



#endif /* CONFIG_H_ */

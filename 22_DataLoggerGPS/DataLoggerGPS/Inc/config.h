/*
 * config.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "can_data.h"

typedef enum {
	UNSIGNED_8BIT 	= 0x00,
	SIGNED_8BIT 	= 0x01,
	UNSIGNED_16BIT 	= 0x02,
	SIGNED_16BIT 	= 0x03,
	ON_OFF_8BIT 	= 0x04, 	//0x00 - OFF, 0xFF - ON
	FLAGS_8BIT 		= 0x05,		//look into documentation for bits meaning definition
	FLAGS_16BIT 	= 0x06,		//look into documentation for bits meaning definition
	CUSTOM_16BIT 	= 0xFE,	//look into documentation
	CUSTOM_8BIT 	= 0xFF		//look into documentation
} ValueType_TypeDef;

typedef struct {
	uint8_t 			DLC;
	ValueType_TypeDef 	valueType;
	uint16_t 			multiplier;
	uint16_t 			divider;
	uint16_t 			offset;
	char* 				description;
	char* 				unit_name;
	char* 				comment;
} ConfigChannel_TypeDef;

typedef struct {
	uint16_t 				ID;
	uint8_t 				DLC;
	char* 					module_or_frame_name;
	ConfigChannel_TypeDef* 	channels;
} ConfigFrame_TypeDef;

typedef struct {
	uint16_t 				version;
	uint16_t 				subversion;
	uint16_t 				num_of_frames;
	ConfigFrame_TypeDef* 	frames;
} Config_TypeDef;


typedef enum {
	ConfigDataManager_Status_OK = 0,
	ConfigDataManager_Status_NoConfigFile,
	ConfigDataManager_Status_NoChannel,
	ConfigDataManager_Status_Error
} ConfigDataManager_Status_TypeDef;

typedef struct {
	uint8_t DLCTab;
} ConfigDataManager_TypeDef;

ConfigDataManager_Status_TypeDef ConfigDataManager_init();
ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigData(Config_TypeDef* pRetConfig);
ConfigDataManager_Status_TypeDef ConfigDataManager_checkCorrectnessData(CANData_TypeDef* pData);
ConfigDataManager_Status_TypeDef ConfigDataManager_findChannel(uint16_t ID, uint8_t offset, ConfigChannel_TypeDef* pRetChannel);



#endif /* CONFIG_H_ */

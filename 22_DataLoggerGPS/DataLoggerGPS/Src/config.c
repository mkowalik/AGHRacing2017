/*
 * config_data.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include "config.h"
#include "file_middleware.h"

uint8_t assertCorrectFrame(ConfigDataManager_TypeDef* pSelf, uint8_t id, unit16_t dlc){

	if (id >= CONFIG_MAX_ID_NUMBER){
		return 0;
	}

	if (pSelf->sConfig.framesByID[id] != NULL){
		return 0;
	}

	if (dlc > CONFIG_MAX_DLC_VALUE){
		return 0;
	}

	return 1;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_init(ConfigDataManager_TypeDef* pSelf){

	uint8_t buffer[32];
	uint32_t bytesRead;
	FileSystemMiddleware_Status_TypeDef status;

	if (pSelf->initialised == 0){

		status = FileSystemMiddleware_init();
		if (status != FileSystemMiddleware_Status_OK){
			return ConfigDataManager_Status_Error;
		}

		status = FileSystemMiddleware_open(self->sConfigFileHandler, CONFIG_FILENAME);

		if (status == FileSystemMiddleware_Status_NO_FILE){
			return ConfigDataManager_Status_NoConfigFileError;
		} else if (status != FileSystemMiddleware_Status_OK){
			return ConfigDataManager_Status_Error;
		}

		status = FileSystemMiddleware_readData(&(pSelf->sConfigFileHandler), buffer, 6, &bytesRead);
		if ((bytesRead < 6) || (status != FileSystemMiddleware_Status_OK)) return ConfigDataManager_Status_ConfigFileDataError;

		pSelf->sConfig.version    		= (buffer[0] & 0xFF) << 8 | (buffer[1] & 0xFF);
		pSelf->sConfig.subversion 		= (buffer[2] & 0xFF) << 8 | (buffer[3] & 0xFF);

		if ((pSelf->sConfig.version != CONFIG_FILE_USED_VERSION) || (pSelf->sConfig.subversion != CONIG_FILE_USED_SUBVERSION)){
			return ConfigDataManager_Status_ConfigFileWrongVersionError;
		}

		pSelf->sConfig.num_of_frames	= (buffer[4] & 0xFF) << 8 | (buffer[5] & 0xFF);

		for (uint16_t i=0; i<pSelf->sConfig.num_of_frames; i++){

			status = FileSystemMiddleware_readData(&(pSelf->sConfigFileHandler), buffer, 3+21, &bytesRead);
			if ((bytesRead < 3+21) || (status != FileSystemMiddleware_Status_OK)) return ConfigDataManager_Status_ConfigFileDataError;

			pSelf->sConfig.frames[i].ID = (buffer[0] & 0xFF) << 8 | (buffer[1] & 0xFF);
			pSelf->sConfig.frames[i].DLC = buffer[2];

			if (assertCorrectFrame(pSelf, pSelf->sConfig.frames[i].ID, pSelf->sConfig.frames[i].DLC)){
				return ConfigDataManager_Status_ConfigFileDataError;
			}

			pSelf->sConfig.framesByID[pSelf->sConfig.frames[i].ID] = &(pSelf->sConfig.frames[i]);

			for (uint8_t j=0; j<pSelf->sConfig.frames[i].DLC; ){

				status = FileSystemMiddleware_readData(&(pSelf->sConfigFileHandler), buffer, 7+21, &bytesRead);
				if ((bytesRead < 7+21) || (status != FileSystemMiddleware_Status_OK)) return ConfigDataManager_Status_ConfigFileDataError;

				pSelf->sConfig.frames[i].channels[j].valueType 	= buffer[0];
				pSelf->sConfig.frames[i].channels[j].multiplier = (buffer[1] & 0xFF) << 8 | (buffer[2] & 0xFF);
				pSelf->sConfig.frames[i].channels[j].divider 	= (buffer[3] & 0xFF) << 8 | (buffer[4] & 0xFF);
				pSelf->sConfig.frames[i].channels[j].offset 	= (buffer[5] & 0xFF) << 8 | (buffer[6] & 0xFF);

				status = FileSystemMiddleware_readData(&(pSelf->sConfigFileHandler), buffer, 21, &bytesRead);
				if ((bytesRead < 21) || (status != FileSystemMiddleware_Status_OK)) return ConfigDataManager_Status_ConfigFileDataError;
				status = FileSystemMiddleware_readData(&(pSelf->sConfigFileHandler), buffer, 21, &bytesRead);
				if ((bytesRead < 21) || (status != FileSystemMiddleware_Status_OK)) return ConfigDataManager_Status_ConfigFileDataError;

				if ((pSelf->sConfig.frames[i].channels[j].valueType & CONFIG_16_BIT_TYPE_flag) == 0){
					j += 1;
				} else {
					j += 2;
				}
			}
		}

		pSelf->initialised = 1;

	}

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_getConfigPointer(ConfigDataManager_TypeDef* pSelf, Config_TypeDef** ppRetConfig){

	if (pSelf->initialised == 0){
		return ConfigDataManager_Status_NotInitialised;
	}

	*(ppRetConfig) = &(pSelf->sConfig);

	return ConfigDataManager_Status_OK;
}

ConfigDataManager_Status_TypeDef ConfigDataManager_getIDsList(ConfigDataManager_TypeDef* pSelf, uint16_t* pRetIDsBuffer, uint16_t bufferSize, uint16_t* pIDsWritten){

	if (pSelf->initialised == 0){
		return ConfigDataManager_Status_NotInitialised;
	}

	for (uint16_t i=0; i<min(bufferSize, pSelf->sConfig.num_of_frames); i++){

		pRetIDsBuffer[i] = pSelf->sConfig.frames[i].ID;

	}

	(*pIDsWritten) = min(bufferSize, pSelf->sConfig.num_of_frames);

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_checkCorrectnessData(ConfigDataManager_TypeDef* self, CANData_TypeDef* pData){

	if (pSelf->initialised == 0){
		return ConfigDataManager_Status_NotInitialised;
	}

	if (self->sConfig->framesByID[pData->ID] == NULL){
		return ConfigDataManager_Status_WrongID;
	}

	if (self->sConfig->framesByID[pData->ID]->DLC != pData->DLC){
		return ConfigDataManager_Status_WrongDLC;
	}

	return ConfigDataManager_Status_OK;

}

ConfigDataManager_Status_TypeDef ConfigDataManager_findChannel(ConfigDataManager_TypeDef* pSelf, uint16_t ID, uint8_t offset, ConfigChannel_TypeDef* pRetChannel){

	if (pSelf->initialised == 0){
		return ConfigDataManager_Status_NotInitialised;
	}

	if (offset > (CONFIG_MAX_DLC_VALUE-1)){
		return ConfigDataManager_Status_WrongOffsetError;
	}

	static ConfigFrame_TypeDef* pFrame = pSelf->sConfig.framesByID[ID];

	for (uint8_t i=0; i<pFrame->DLC; ){

		if (i > offset){
			return ConfigDataManager_Status_WrongOffsetError;
		} else if (i == offset){
			pRetChannel = &(pFrame->channels[i]);
			return ConfigDataManager_Status_OK;
		}

		if ((pFrame->channels[i].valueType & CONFIG_16_BIT_TYPE_flag) == 0){
			i += 1;
		} else {
			i += 2;
		}
	}

	return ConfigDataManager_Status_WrongOffsetError;

}

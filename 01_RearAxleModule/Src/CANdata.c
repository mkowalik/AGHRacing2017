/*
 * CAN_data.c
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#include <CANhandler.h>
#include "CANdata.h"
#include "CANframe.h"

CANdata_handle_t CANdata_Init(void (* eventHdlr)(CANdata_t * data)){
	static CANdata_handle_t handle = {	.dataElementsNumber = 0,
										.eventHandler = eventHdlr,
										.frameHandle = CANframe_Init()};
	return handle;
}

CANdata_StatusTypeDef CANdata_DeInit(CANdata_handle_t * handle){
	handle.dataElementsNumber = 0;
	return CANdata_OK;
}

CANdata_StatusTypeDef CANdata_InitData(CANdata_handle_t * handle, CANdata_t * data, CANdata_direction dir){

	for(uint8_t elementCounter = 0; elementCounter < handle->dataElementsNumber; elementCounter++){
		if(handle->dataElements[elementCounter] == data){
			return CANdata_ERROR;
		}
	}
	CANframe_InitFrame(handle->frameHandle, )


	handle->dataElements[handle->dataElementsNumber] = data;
	handle->dataElementsNumber++;
	data->dir = 0;
	data->validData = 0;
	data->data = 0;
}

CANdata_StatusTypeDef CANdata_DeInitData(CANdata_handle_t * handle, CANdata_t * data){
	for(uint8_t elementCounter = 0; elementCounter < handle->dataElementsNumber; elementCounter++){
		if(handle->dataElements[elementCounter] == data){
			for(;elementCounter < handle->dataElementsNumber - 1; elementCounter++){
				handle->dataElements[elementCounter] = handle->dataElements[elementCounter + 1];
			}
			handle->dataElementsNumber--;
			return CANdata_OK;
		}
	}
	return CANdata_ERROR;
}

CANdata_StatusTypeDef CANdata_Handler(CANdata_handle_t * handle);


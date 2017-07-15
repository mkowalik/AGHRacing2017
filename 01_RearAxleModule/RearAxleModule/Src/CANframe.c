/*
 * CANframe.c
 *
 *  Created on: Jul 12, 2017
 *      Author: Wojciech
 */

#include "CANframe.h"

CANframe_handle_t CANframe_Init(void (* eventHdlr)(CANframe_t * frame)){
	static CANframe_handle_t handle = {.framesNumber = 0,
								.eventHandler = eventHdlr};
	return handle;
}

Fifo_StatusTypeDef CANframe_InitFrame(CANframe_handle_t * handler, CANframe_t * frame, CANframe_directionTypeDef direction){
	if(handler->usedCANFramesNumber == MAX_CAN_FRAMES){
		return CANdata_ERROR;
	}
	handler->usedCANFrames[handler->usedCANFramesNumber].frame = frame;
	handler->usedCANFrames[handler->usedCANFramesNumber].lastSendTime = HAL_GetTick();
	handler->usedCANFrames[handler->usedCANFramesNumber].dir = direction;
	handler->usedCANFrames[handler->usedCANFramesNumber].enabled = 1;
	handler->usedCANFramesNumber ++;
	return CANdata_OK;
}

Fifo_StatusTypeDef CANframe_DeInitFrame(CANframe_handle_t * handler, CANframe_t *frame){
	for(uint16_t frameCounter = 0; frameCounter < MAX_CAN_FRAMES; frameCounter++){
		if(handler->usedCANFrames[frameCounter].frame == frame){
			handler->usedCANFrames[frameCounter].enabled = 0;
			handler->usedCANFramesNumber--;
			return CANdata_OK;
		}
	}
	return CANdata_ERROR;
}

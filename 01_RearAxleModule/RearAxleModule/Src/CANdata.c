/*
 * CAN_data.c
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#include <CANhandler.h>

CANdata_handle_t CANdata_Init(){
	CANdata_handle_t handler;
	static CANdata_frame_t usedFrames[MAX_CAN_FRAMES];

	handler.usedCANFrames = usedFrames;
	handler.usedCANFramesNumber = 0;
	return handler;
}

Fifo_StatusTypeDef CANdata_InitFrame(CANdata_handle_t * handler, CANframe_t * frame, CANframe_directionTypeDef direction){
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

Fifo_StatusTypeDef CANdata_DeInitFrame(CANdata_handle_t * handler, CANframe_t *frame){
	for(uint16_t frameCounter = 0; frameCounter < MAX_CAN_FRAMES; frameCounter++){
		if(handler->usedCANFrames[frameCounter].frame == frame){
			handler->usedCANFrames[frameCounter].enabled = 0;
			handler->usedCANFramesNumber--;
			return CANdata_OK;
		}
	}
	return CANdata_ERROR;
}

Fifo_StatusTypeDef CANdata_Handler(CANdata_handle_t * handler){
	return CANdata_OK;
}

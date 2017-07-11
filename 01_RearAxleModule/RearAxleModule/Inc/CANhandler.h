/*
 * CAN_data.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#ifndef CANHANDLER_H_
#define CANHANDLER_H_

#include "stm32f0xx_hal.h"
#include "CAN_frames.h"

#define MAX_CAN_FRAMES	50

typedef struct{
	CANframe_t *frame;
	CANframe_directionTypeDef dir;
	uint32_t lastSendTime;
	uint8_t enabled;
}CANhandler_frame_t;

typedef struct{
	uint8_t usedCANFramesNumber;
	CANhandler_frame_t *usedCANFrames;
}CANhandler_handle_t;

typedef enum{
	CANhandler_OK       = 0x00,
	CANhandler_ERROR    = 0x01,
	CANhandler_BUSY     = 0x02,
	CANhandler_TIMEOUT  = 0x03
}CANhandler_StatusTypeDef;

CANhandler_handle_t CANhandler_Init();
CANhandler_StatusTypeDef CANhandler_InitFrame(CANframe_t frame, CANframe_directionTypeDef);
CANhandler_StatusTypeDef CANhandler_Handler(void);



#endif /* CANHANDLER_H_ */

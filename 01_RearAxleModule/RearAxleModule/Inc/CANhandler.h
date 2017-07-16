/*
 * CAN_data.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#ifndef CANHANDLER_H_
#define CANHANDLER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "can.h"
 //USER DEFINED FRAMES
#define CAN_STOP_LIGHT_RECEIVE
//#define CAN_ACC_TRANSMIT
//#define CAN_GYR_TRANSMIT

#define MAX_DATA_ELEMENTS	50 //user set
#define CAN_FRAMES			48




 typedef struct{
 	uint32_t data;
 	uint32_t updateTime;
 	uint8_t dataValid;
 }CANhandler_data_t;

 typedef struct{
 	CANhandler_data_t * data[MAX_DATA_ELEMENTS];
 }CANhandler_handle_t;

 extern volatile CANhandler_handle_t handle;
 //USER DEFINED FRAMES END

#define GET_CAN_DATA_VALIDATION(_data_)		((( handle ).data[_data_##_LOCAL_ID])->dataValid)
#define GET_CAN_DATA(_data_)				((( handle ).data[_data_##_LOCAL_ID])->data)
#define SET_CAN_DATA(_data_, _value_)		((( handle ).data[_data_##_LOCAL_ID])->data = (_value_))

#define FAM_ACC_STOP_SW_PERIOD		((uint32_t) 10)
#define FAM_ACC_STOP_SW_ID			((uint16_t) 0x100U)

#define FAM_DSI_PERIOD				((uint32_t) 1000)
#define FAM_DSI_ID					((uint16_t) 0x110U)

#define RAM_ACC_PERIOD				((uint32_t) 10)
#define RAM_ACC_ID					((uint16_t) 0x180U)

#define RAM_GYRO_PERIOD				((uint32_t) 10)
#define RAM_GYRO_ID					((uint16_t) 0x181U)

#define DATA_STOP_LIGHT_LOCAL_ID	0
#define DATA_STOP_SW_BYTEOFFSET		((uint8_t) 6)

#define DATA_R_ACC_X_LOCAL_ID		1
#define DATA_R_ACC_X_BYTEOFFSET		((uint8_t) 0)

#define DATA_R_ACC_Y_LOCAL_ID		2
#define DATA_R_ACC_Y_BYTEOFFSET		((uint8_t) 2)

#define DATA_R_ACC_Z_LOCAL_ID		3
#define DATA_R_ACC_Z_BYTEOFFSET		((uint8_t) 4)

#define DATA_R_GYR_X_LOCAL_ID		4
#define DATA_R_GYR_X_BYTEOFFSET		((uint8_t) 0)

#define DATA_R_GYR_Y_LOCAL_ID		5
#define DATA_R_GYR_Y_BYTEOFFSET		((uint8_t) 2)

#define DATA_R_GYR_Z_LOCAL_ID		6
#define DATA_R_GYR_Z_BYTEOFFSET		((uint8_t) 4)

 void CANhandler_Init(void);

 void CANhandler_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* CANHANDLER_H_ */

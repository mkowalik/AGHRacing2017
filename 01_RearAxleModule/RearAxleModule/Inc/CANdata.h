/*
 * CANdata.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#ifndef CANDATA_H_
#define CANDATA_H_

typedef enum{
	CANdata_RECEIVE			= 0x00,
	CANdata_SEND			= 0x01,
	CANdata_RECEIVE_SEND 	= 0x02
}CANdata_directionTypeDef;

typedef enum{
	GearUpSW		= 0x00,
	GearDownSW		= 0x01,
	ClutchSW		= 0x02,
	NeutralSW		= 0x03,
	StWheelSW1		= 0x04,
	StWheelSW2		= 0x05,
	StWheelSW3		= 0x06,
	StWheelSW4		= 0x07,
	StWheelSW5		= 0x08,
	StWheelSW6		= 0x09,
	StWheelSW7		= 0x0A,
	StWheelSW8		= 0x0B,
	StWheelAn1		= 0x0C,
	StWheelAn2		= 0x0D,
	StWheelAn3		= 0x0E,
	StWheelAn4		= 0x0F
}CANdata_dataTypeDef;

typedef enum{
	CANdata_OK       = 0x00,
	CANdata_ERROR    = 0x01,
	CANdata_BUSY     = 0x02,
	CANdata_TIMEOUT  = 0x03
}CANdata_StatusTypeDef;

CANdata_handle_t CANdata_Init();
CANdata_StatusTypeDef CANdata_InitData(CANdata_dataTypeDef data, CANdata_directionTypeDef dir);
CANdata_StatusTypeDef CANdata_Handler(void);

#endif /* CANDATA_H_ */

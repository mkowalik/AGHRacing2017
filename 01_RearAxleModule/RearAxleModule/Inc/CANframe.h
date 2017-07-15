/*
 * CANframe.h
 *
 *  Created on: Jul 11, 2017
 *      Author: Wojciech
 */

#ifndef CANFRAME_H_
#define CANFRAME_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f0xx_hal.h"

#define PERIOD_ON_EVENT	((uint32_t) 0)
#define MAX_CAN_FRAMES	50

typedef enum{
	CANframe_RECEIVE		= 0x00,
	CANframe_SEND			= 0x01,
	CANframe_RECEIVE_SEND 	= 0x02
}CANframe_directionTypeDef;

typedef struct{
	static uint32_t period;
	static uint16_t ID;
	int32_t	Readvalue;
	int32_t WriteValue;
}CANframe_t;

typedef struct{
	CANframe_t * frames[MAX_CAN_FRAMES];
	uint8_t framesNumber;
	void (*eventHandler)(CANframe_t * frame);
}CANframe_handle_t;

#define GEAR_UP_SW_PERIOD			PERIOD_ON_EVENT
#define GEAR_UP_SW_ID				((uint16_t) 0x000U)
#define GEAR_UP_SW_DLC				((uint8_t) 1)

#define GEAR_DOWN_SW_PERIOD			PERIOD_ON_EVENT
#define GEAR_DOWN_SW_ID				((uint16_t) 0x001U)

#define CLUTCH_SW_PERIOD			PERIOD_ON_EVENT
#define CLUTCH_SW_ID				((uint16_t) 0x002U)

#define NEUTRAL_SW_PERIOD			PERIOD_ON_EVENT
#define NEUTRAL_SW_ID				((uint16_t) 0x003U)

#define ST_WHEEL_18_SW_PERIOD		((uint32_t) 10)
#define ST_WHEEL_18_SW_ID			((uint16_t) 0x004U)

#define ST_WHEEL_14_AN_PERIOD		((uint32_t) 10)
#define ST_WHEEL_14_AN_ID			((uint16_t) 0x005U)

#define ST_WHEEL_DSI_PERIOD			((uint32_t) 1000)
#define ST_WHEEL_DSI_ID				((uint16_t) 0x010U)

#define GMU_GEAR_CLUTCH_PERIOD		((uint32_t) 10)
#define GMU_GEAR_CLUTCH_ID			((uint16_t) 0x080U)

#define GMU_DSI_PERIOD				((uint32_t) 1000)
#define GMU_DSI_ID					((uint16_t) 0x090U)

#define FAM_ACC_STOP_SW_PERIOD		((uint32_t) 10)
#define FAM_ACC_STOP_SW_ID			((uint16_t) 0x100U)

#define FAM_GYRO_PERIOD				((uint32_t) 10)
#define FAM_GYRO_ID					((uint16_t) 0x101U)

#define FAM_DSI_PERIOD				((uint32_t) 1000)
#define FAM_DSI_ID					((uint16_t) 0x110U)

#define RAM_ACC_PERIOD				((uint32_t) 10)
#define RAM_ACC_ID					((uint16_t) 0x180U)

#define RAM_GYRO_PERIOD				((uint32_t) 10)
#define RAM_GYRO_ID					((uint16_t) 0x181U)

#define RAM_DSI_PERIOD				((uint32_t) 1000)
#define RAM_DSI_ID					((uint16_t) 0x190U)

#define WHEEL_FL_DATA_PERIOD		((uint32_t) 10)
#define WHEEL_FL_DATA_ID			((uint16_t) 0x200U)

#define WHEEL_FL_TIRE_PERIOD		((uint32_t) 10)
#define WHEEL_FL_TIRE_ID			((uint16_t) 0x201U)

#define WHEEL_FL_DSI_PERIOD			((uint32_t) 1000)
#define WHEEL_FL_DSI_ID				((uint16_t) 0x210U)

#define WHEEL_FR_DATA_PERIOD		((uint32_t) 10)
#define WHEEL_FR_DATA_ID			((uint16_t) 0x280U)

#define WHEEL_FR_TIRE_PERIOD		((uint32_t) 10)
#define WHEEL_FR_TIRE_ID			((uint16_t) 0x281U)

#define WHEEL_FR_DSI_PERIOD			((uint32_t) 1000)
#define WHEEL_FR_DSI_ID				((uint16_t) 0x290U)

#define WHEEL_RL_DATA_PERIOD		((uint32_t) 10)
#define WHEEL_RL_DATA_ID			((uint16_t) 0x300U)

#define WHEEL_RL_TIRE_PERIOD		((uint32_t) 10)
#define WHEEL_RL_TIRE_ID			((uint16_t) 0x301U)

#define WHEEL_RL_DSI_PERIOD			((uint32_t) 1000)
#define WHEEL_RL_DSI_ID				((uint16_t) 0x310U)

#define WHEEL_RR_DATA_PERIOD		((uint32_t) 10)
#define WHEEL_RR_DATA_ID			((uint16_t) 0x380U)

#define WHEEL_RR_TIRE_PERIOD		((uint32_t) 10)
#define WHEEL_RR_TIRE_ID			((uint16_t) 0x381U)

#define WHEEL_RR_DSI_PERIOD			((uint32_t) 1000)
#define WHEEL_RR_DSI_ID				((uint16_t) 0x390U)

#define FUEL_LEVEL_SENS_PERIOD		((uint32_t) 10)
#define FUEL_LEVEL_SENS_ID			((uint16_t) 0x400U)

#define FUEL_LEVEL_DSI_PERIOD		((uint32_t) 1000)
#define FUEL_LEVEL_DSI_ID			((uint16_t) 0x410U)

#define DASH_IR_SW_PERIOD			PERIOD_ON_EVENT
#define DASH_IR_SW_ID				((uint16_t) 0x480U)

#define DASH_DSI_PERIOD				((uint32_t) 1000)
#define DASH_DSI_ID					((uint16_t) 0x490U

#define DATALOGGER_DSI_PERIOD		((uint32_t) 1000)
#define DATALOGGER_DSI_ID			((uint16_t) 0x510U)

#define TELEMETRY_DSI_PERIOD		((uint32_t) 1000)
#define TELEMETRY_DSI_ID			((uint16_t) 0x590U)

#define EMU_FRAME1_PERIOD			((uint32_t) 50)
#define EMU_FRAME1_ID				((uint16_t) 0x600U)

#define EMU_FRAME2_PERIOD			((uint32_t) 50)
#define EMU_FRAME2_ID				((uint16_t) 0x601U)

#define EMU_FRAME3_PERIOD			((uint32_t) 50)
#define EMU_FRAME3_ID				((uint16_t) 0x602U)

#define EMU_FRAME4_PERIOD			((uint32_t) 50)
#define EMU_FRAME4_ID				((uint16_t) 0x603U)

#define EMU_FRAME5_PERIOD			((uint32_t) 50)
#define EMU_FRAME5_ID				((uint16_t) 0x604U)

#define EMU_FRAME6_PERIOD			((uint32_t) 50)
#define EMU_FRAME6_ID				((uint16_t) 0x605U)

#define EMU_FRAME7_PERIOD			((uint32_t) 50)
#define EMU_FRAME7_ID				((uint16_t) 0x606U)

#define PMU_FRAME1_PERIOD			((uint32_t) 50)
#define PMU_FRAME1_ID				((uint32_t) 0x700U)

#define PMU_FRAME2_PERIOD			((uint32_t) 16)
#define PMU_FRAME2_ID				((uint32_t) 0x701U)

#define PMU_FRAME3_PERIOD			((uint32_t) 0)
#define PMU_FRAME3_ID				((uint32_t) 0x702U)

#define PMU_FRAME4_PERIOD			((uint32_t) 0)
#define PMU_FRAME4_ID				((uint32_t) 0x703U)

#define PMU_FRAME5_PERIOD			((uint32_t) 16)
#define PMU_FRAME5_ID				((uint32_t) 0x704U)

#define PMU_FRAME6_PERIOD			((uint32_t) 16)
#define PMU_FRAME6_ID				((uint32_t) 0x705U)

#define PMU_FRAME7_PERIOD			((uint32_t) 0)
#define PMU_FRAME7_ID				((uint32_t) 0x706U)

#define PMU_FRAME8_PERIOD			((uint32_t) 0)
#define PMU_FRAME8_ID				((uint32_t) 0x707U)

#if defined STEERING_WHEEL_FRAMES

	CANframe_t GearUpSw = {
		.period 	= GEAR_UP_SW_PERIOD,
		.ID 		= GEAR_UP_SW_ID
	};

	CANframe_t GearDownSw = {
		.period 	= GEAR_DOWN_SW_PERIOD,
		.ID 		= GEAR_DOWN_SW_ID
	};

	CANframe_t ClutchSw = {
		.period 	= CLUTCH_SW_PERIOD,
		.ID 		= CLUTCH_SW_ID
	};

	CANframe_t NeutralSw = {
		.period 	= NEUTRAL_SW_PERIOD,
		.ID 		= NEUTRAL_SW_ID
	};

	CANframe_t StWheel18 = {
		.period 	= ST_WHEEL_18_SW_PERIOD,
		.ID 		= ST_WHEEL_18_SW_ID
	};

	CANframe_t SwWheel14 = {
		.period 	= ST_WHEEL_14_AN_PERIOD,
		.ID 		= ST_WHEEL_14_AN_ID
	};

	CANframe_t StWheelDSI = {
		.period 	= ST_WHEEL_DSI_PERIOD,
		.ID 		= ST_WHEEL_DSI_ID
	};

#endif

#if defined GMU_FRAMES

#endif

#if defined FRONT_AXLE_MODULE_FRAMES

#endif

#if defined REAR_AXLE_MODULE_FRAMES

#endif

#if defined WHEEL_MODULE_FL_FRAMES

#endif

#if defined WHEEL_MODULE_FR_FRAMES

#endif

#if defined WHEEL_MODULE_RL_FRAMES

#endif

#if defined WHEEL_MODULE_RR_FRAMES

#endif

#if defined FUEL_LEVEL_SENSOR_FRAMES

#endif

#if defined DASHBOARD_FRAMES

#endif

#if defined DATALOGGER_FRAMES

#endif

#if defined TELEMETRY_FRAMES

#endif

#if defined EMU_FRAMES

#endif

#if defined PMU_FRAMES

#endif


#ifdef __cplusplus
}
#endif

#endif /* CANFRAME_H_ */

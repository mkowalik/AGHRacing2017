/*
 * main.cpp
 *
 *  Created on: 03.06.2017
 *      Author: Marek
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include "main.h"
#include "init.h"
#include "stm32f0xx_hal.h"
#include "LCD_UC1608.h"
#include "tim.h"
#include "can.h"
#include "stm32f0xx_hal_can.h"

#define ADD_STATE(var, state)		var = (SteeringWheelState)(var | state)
#define REMOVE_STATE(var, state)	var = (SteeringWheelState)(var & (~state))
#define ELAPSED_TIME(prev_tick)		(HAL_GetTick() - prev_tick)

enum SteeringWheelState {
	IDLE = 0x00,
	GEAR_UP = 0x01,
	GEAR_DOWN = 0x02,
	TICK_100MS = 0x40,
	TICK_1S = 0x80
};

const uint16_t SW_DELAY = 200;
volatile SteeringWheelState stwh_state = IDLE;
CanTxMsgTypeDef tx_msg;

void padleSwitcheHandler(TIM_HandleTypeDef *htim);

int main()
{
	init();

	HAL_Delay(100);
	LCD_Init();

	tx_msg.ExtId = 0;
	tx_msg.RTR = CAN_RTR_DATA;
	tx_msg.IDE = CAN_ID_STD;

	hcan.pTxMsg = &tx_msg;

	static uint32_t tick_100ms = 0, tick_1s = 0;

	while(1) {

		if( ELAPSED_TIME(tick_100ms) >= 100 ) ADD_STATE(stwh_state, TICK_100MS);
		if( ELAPSED_TIME(tick_1s) >= 1000 ) ADD_STATE(stwh_state, TICK_1S);

		if( stwh_state & GEAR_UP) {
			tx_msg.StdId = 0;
			tx_msg.DLC = 1;
			tx_msg.Data[0] = 0xFF;
			REMOVE_STATE(stwh_state, GEAR_UP);
		}

		if( stwh_state & GEAR_DOWN) {
			tx_msg.StdId = 1;
			tx_msg.DLC = 1;
			tx_msg.Data[0] = 0xFF;
			REMOVE_STATE(stwh_state, GEAR_DOWN);
		}

		if( stwh_state & TICK_100MS ) {

			REMOVE_STATE(stwh_state, TICK_100MS);
			tick_100ms = HAL_GetTick();
		}

		if( stwh_state & TICK_1S ) {

			REMOVE_STATE(stwh_state, TICK_1S);
			tick_1s = HAL_GetTick();
		}

	}

}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	if( htim == &htim1 ) {
		padleSwitcheHandler(htim);
	}

}

void padleSwitcheHandler(TIM_HandleTypeDef *htim) {

	static uint32_t gear_up_tick = 0, gear_down_tick = 0;
	static bool gear_up_clicked = false, gear_down_clicked = false;

	if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 ) {
		if( __HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC1OF) ) {
			__HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1OF);
			return;
		}

		uint8_t gear_down_pin = HAL_GPIO_ReadPin(SW_LP_GPIO_Port, SW_LP_Pin);
		if( !gear_down_clicked && !gear_down_pin && (ELAPSED_TIME(gear_down_tick) > SW_DELAY) ) {
			ADD_STATE(stwh_state, GEAR_DOWN);
			gear_down_clicked = true;
			gear_down_tick = HAL_GetTick();
		} else {
			gear_down_clicked = false;
			gear_down_tick = HAL_GetTick();
		}

	} else if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3 ) {
		if( __HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC3OF) ) {
			__HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC3OF);
			return;
		}

		uint8_t gear_up_pin = HAL_GPIO_ReadPin(SW_RP_GPIO_Port, SW_RP_Pin);
		if( !gear_up_clicked && !gear_up_pin && (ELAPSED_TIME(gear_up_tick) > SW_DELAY)) {
			ADD_STATE(stwh_state, GEAR_UP);
			gear_up_clicked = true;
			gear_up_tick = HAL_GetTick();
		} else {
			gear_up_clicked = false;
			gear_up_tick = HAL_GetTick();
		}
	}

}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  while(1)
  {
  }
}

#endif /* MAIN_CPP_ */

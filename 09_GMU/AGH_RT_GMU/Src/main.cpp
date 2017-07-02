/*
 * main.cpp
 *
 *  Created on: 03.06.2017
 *      Author: Marek
 */
#include "main.h"
#include "stm32f0xx_hal.h"
#include "can.h"
#include "stm32f0xx_hal_can.h"
#include <string.h>

#define ADD_STATE(var, state)		var = (GmuState)(var | state)
#define REMOVE_STATE(var, state)	var = (GmuState)(var & (~state))
#define ELAPSED_TIME(prev_tick)		(HAL_GetTick() - prev_tick)


CAN_FilterConfTypeDef can_filter;
CanRxMsgTypeDef rx_msg;

enum GmuState {
	IDLE = 0x00,
	GEAR_UP = 0x01,
	GEAR_DOWN = 0x02,
	TICK_100MS = 0x40,
	TICK_1S = 0x80
};

volatile GmuState gmu_state = IDLE;

void gear_up();
void gear_down();

int main(void)
{
	HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_RESET);

	memset(&can_filter, 0, sizeof(can_filter));
	can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter.FilterActivation = ENABLE;

	hcan.pRxMsg = &rx_msg;

	HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

	static uint32_t tick_100ms = 0, tick_1s = 0;

	while(1) {

		if( ELAPSED_TIME(tick_100ms) >= 100 ) ADD_STATE(gmu_state, TICK_100MS);
		if( ELAPSED_TIME(tick_1s) >= 1000 ) ADD_STATE(gmu_state, TICK_1S);

		if( gmu_state & GEAR_UP) {
			gear_up();
			REMOVE_STATE(gmu_state, GEAR_UP);
		}

		if( gmu_state & GEAR_DOWN) {
			gear_down();
			REMOVE_STATE(gmu_state, GEAR_DOWN);
		}

		if( gmu_state & TICK_100MS ) {

			REMOVE_STATE(gmu_state, TICK_100MS);
			tick_100ms = HAL_GetTick();
		}

		if( gmu_state & TICK_1S ) {

			REMOVE_STATE(gmu_state, TICK_1S);
			tick_1s = HAL_GetTick();
		}

	}
}

void gear_up()
{
	// start gear up
	HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_SET);
	// valve delay
	HAL_Delay(20);
	// actuator travel delay
	HAL_Delay(80);
	// gear cut
	HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_SET);
	// gear cut delay
	HAL_Delay(100);
	// gear cut off
	HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_RESET);
	// gear up off
	HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_RESET);
}

void gear_down()
{
	// start gear down
	HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_SET);
	// valve delay
	HAL_Delay(20);
	// actuator travel delay
	HAL_Delay(80);
	// clutch on
	HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_SET);
	// clutch delay
	HAL_Delay(100);
	// clutch off
	HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_RESET);
	// gear down off
	HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_RESET);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	if(rx_msg.StdId == 0 && rx_msg.DLC == 1 && rx_msg.Data[0] == 0xFF)
	{
		ADD_STATE(gmu_state, GEAR_UP);
	} else if(rx_msg.StdId == 1 && rx_msg.DLC == 1 && rx_msg.Data[0] == 0xFF)
	{
		ADD_STATE(gmu_state, GEAR_DOWN);
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(const char * file, int line)
{
  while(1)
  {
  }
}

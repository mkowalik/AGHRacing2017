/*
 * stop_pedal.c
 *
 *  Created on: May 11, 2017
 *      Author: Wojciech
 */

#include "gpio.h"
#include "main.h"

/**
  * @brief  Read stop pedal value.
  * @note	Pedal is pulled-up, so positive value on pin means that pedal is not pressed down.
  * @retval	0 - pedal not pressed, 1 - pedal pressed.
  */
uint8_t StopPedal_Read(void){

	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(STOP_PEDAL_GPIO_Port, STOP_PEDAL_Pin)){
		return 0xFF;
	}
	return 0;
}

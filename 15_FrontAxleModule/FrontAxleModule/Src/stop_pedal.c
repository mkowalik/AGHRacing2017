/*
 * stop_pedal.c
 *
 *  Created on: May 11, 2017
 *      Author: Wojciech
 */

#include "gpio.h"
#include "main.h"
#include "stop_pedal.h"

#define DEBOUNCE_TIME	50
/**
  * @brief  Read stop pedal value.
  * @note	Pedal is pulled-up, so positive value on pin means that pedal is not pressed down.
  * @retval	0 - pedal not pressed, 1 - pedal pressed.
  */
bool StopPedal_Read(void){
	static uint32_t pedal_read_time;
	static bool prev_read;
	static bool prev_return;
	bool read;

	if(pedal_read_time < HAL_GetTick() - DEBOUNCE_TIME){
		// Read new data
		if(GPIO_PIN_SET == HAL_GPIO_ReadPin(STOP_PEDAL_GPIO_Port, STOP_PEDAL_Pin)){
			read = true;
		}
		else{
			read = false;
		}
		pedal_read_time = HAL_GetTick();

		if(read == prev_read){
			prev_return = read;
			return read;
		}
		prev_read = read;
	}
	return prev_return;
}

void StopPedal_can_data_extract(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size, void * value){

	if(*((bool *)value)){
		*data_ptr = 0xFF;
	}
	else{
		*data_ptr = 0x00;
	}
}

float StopPedal_can_data_calc(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size){

	if(*data_ptr == 0xFF){
		return 1;
	}
	else{
		return 0;
	}
}

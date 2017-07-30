/*
 * stop_light.c
 *
 *  Created on: May 11, 2017
 *      Author: Wojciech
 */

#include "stop_light.h"
#include "main.h"
#include "gpio.h"

typedef enum StopLight_value{
	OFF = 0,
	ON
}StopLight_value_t;

static void StopLight_Write(StopLight_value_t value){

	switch(value){

	case OFF:
		HAL_GPIO_WritePin(STOP_LIGHT_GPIO_Port, STOP_LIGHT_Pin, GPIO_PIN_RESET);
		break;

	case ON:
		HAL_GPIO_WritePin(STOP_LIGHT_GPIO_Port, STOP_LIGHT_Pin, GPIO_PIN_SET);
		break;

	}

}

void StopLight_Set(void){
	StopLight_Write(ON);
}

void StopLight_Clr(void){
	StopLight_Write(OFF);
}


float StopLight_can_data_calc(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size){

	if(*data_ptr == 0xFF){
		return 1;
	}
	else{
		return 0;
	}
}

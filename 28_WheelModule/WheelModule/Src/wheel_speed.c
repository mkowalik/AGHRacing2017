/*
 *
 ******************************************************************************
 * @file    wheel_speed.c
 * @brief  	Wheel speed sensor routines.
 ******************************************************************************
 *  Created on: 22.05.2017
 *  Author: Leo Brockhuis
 */

/* Includes ------------------------------------------------------------------*/
#include "wheel_speed.h"
/* Defines -------------------------------------------------------------------*/
#define _TOOTH_THR 1000 //!< Is_tooth threshold, has to be experimentally set
#define _TIME_STP 1000 //!< Time between speed updates
#define _TOOTH_NUM 18 //!< Number of teeth on the wheel
/* Private Variables------------------------------------------------------------*/
float wh_speed; 	/*!< Current wheel speed in RPM //Move to .h for testing w-o CAN */
uint8_t curr; 		//!< Is a tooth currently seen?
uint8_t prev;		//!< Was a tooth previously seen?
uint16_t tooth_cnt;	//!< How many teeth have passed since since the last speed output?
uint32_t curr_t;	//!< Time at HAL_ADC_ConvCpltCallback call
uint32_t prev_t;	//!< Time at previous HAL_ADC_ConvCpltCallback call

/* Function definitions ----------------------------------------------------------*/
/**
  * @brief Initializes wheel speed ADC
  * @param  none
  * @retval none
  */
void WHEEL_SPEED_Init(ADC_HandleTypeDef* hadc){
	HAL_ADC_Start_IT(hadc);
}
/**
  * @brief  Hal sensor conversion complete callback with wheel speed output
  * @param  hadc: Hall ADC handle
  * @retval None
  * NOTE: Lacks CAN functionality
  */
void	HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	curr = 0;
	if(HAL_ADC_GetValue(hadc) > _TOOTH_THR){
		curr = 1;
	}
	if(curr != prev){
		tooth_cnt = tooth_cnt + 1;
	}
	curr_t = HAL_GetTick();
	if(curr_t - prev_t >= _TIME_STP){
		/*!< The next line assumes HAL_GetTick() returns in ms */
		wh_speed = (float)tooth_cnt/(curr_t - prev_t) * 6e4/(float)_TOOTH_NUM;
		prev_t = curr_t;
		tooth_cnt = 0;
	}
	prev = curr;
}

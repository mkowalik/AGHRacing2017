/*
 *
 ******************************************************************************
 * @file    wheel_speed.h
 * @brief  	This file contains the defines of the wheel speed sensor routines.
 ******************************************************************************
 *  Created on: 22.05.2017
 *  Author: Leo Brockhuis
 */


#ifndef WHEEL_SPEED_H_
#define WHEEL_SPEED_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Function Prototypes--------------------------------------------------------*/
void WHEEL_SPEED_Init(ADC_HandleTypeDef* hadc); //!< initializes the ADC conversion for the WHEEL_SPEED sensor

#endif /* WHEEL_SPEED_H_ */

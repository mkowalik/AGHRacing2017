/*
 *
 ******************************************************************************
 * @file    GPS.h
 * @brief  	This file contains the defines of the GPS configuration file
 ******************************************************************************
 *  Created on: 24.05.2017
 *  Author: Maciej Jarosz
 */


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* Function Prototypes--------------------------------------------------------*/
void GPS_Configure_Init(UART_HandleTypeDef* huart1);
void GPS_Start_Transmit(UART_HandleTypeDef* huart1);
void GPS_Check_Data(UART_HandleTypeDef* huart1, uint8_t Pos11, uint8_t Pos12, uint8_t Pos21, uint8_t Pos22);
void GPS_Go_Sleep(UART_HandleTypeDef* huart1);

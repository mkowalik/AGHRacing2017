/*
 *
 ******************************************************************************
 * @file    GPS.c
 * @brief  	GPS configuration file
 ******************************************************************************
 *  Created on: 24.05.2017
 *  Author: Maciej Jarosz
 */

/* Includes ------------------------------------------------------------------*/
#include "GPS.h"
/* Defines -------------------------------------------------------------------*/

/* Private Variables------------------------------------------------------------*/
uint8_t Received[40]; // Array to receive data
uint8_t Data[60]; // Array to send data
uint8_t Size = 0; // Size of the sending and receiving data
uint8_t Sleep = 0; // If device is in standby mode or not
uint8_t Pos11; // 1. position read from new message
uint8_t Pos12; // 2. position read from new message
uint8_t Pos21 = 0; // 1. position read from old message
uint8_t Pos22 = 0; // 2. position read from old message
uint16_t Cnt = 0; // How long car was in one place

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

/* Function definitions ----------------------------------------------------------*/
void GPS_Check_Data(UART_HandleTypeDef*,uint8_t,uint8_t,uint8_t,uint8_t);
void GPS_Go_Sleep(UART_HandleTypeDef*);
void GPS_ArrToNum(uint8_t*);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Count interruptions until 300(5 min)
	 * If position of the car will be the same for 5 minutes Sim28 will go
	 * into Standby mode
	*/
	Cnt++;
	if (Cnt >= 300)
	{
		GPS_Go_Sleep(&huart1);
	}
};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	  // Receiving messages
	  Size = sprintf(Data, "%s\n\r",Received);

	  GPS_ArrToNum(Data);

	  // Waiting for message and write it to Received
	  HAL_UART_Receive(huart, Received, 40,100);
};

void GPS_Configure_Init(UART_HandleTypeDef* huart1){

	  // FULL COLD START - Without verification of correct start
	  Size = sprintf(Data, "$PMTK104*37<CR><LF>");
	  HAL_UART_Transmit(huart1, Data, Size,100);
}

void GPS_Start_Transmit(UART_HandleTypeDef* huart1){
	  // Set SIM28 to send only data about position
	  Size = sprintf(Data, "$PMTK314,0,1,0,0,0,,0,0,0,0,0,0,0,0,0,0,0,1,0*2D<CR><LF>");
	  // Start send data with IT
	  HAL_UART_Transmit(huart1, Data, Size,1);
	  HAL_UART_Receive_IT(huart1, Received, 40);
}

void GPS_ArrToNum(uint8_t* Data)
{
	uint8_t data_tmp = 0;
	uint8_t	array_mult = 1;
	// Convert numeric data about the position to integer
		  for(uint8_t lc_count = 22; lc_count > 18; lc_count--)
		      {
		          data_tmp = Data[lc_count];
		          data_tmp *= array_mult;
		          Pos11 = Pos11 + data_tmp;
		          array_mult *= 10;
		      }

		  // Convert numeric data about the position to integer
		  // eg. ([4]*1)+([3]*10)+([2]*100)+([1]*1000)+([0]*10000)=1234
		  array_mult = 1;
		  for(uint8_t lc_count = 34; lc_count > 30; lc_count--)
		      {
		          data_tmp = Data[lc_count];
		          data_tmp *= array_mult;
		          Pos12 = Pos12 + data_tmp;
		          array_mult *= 10;
		      }

		  GPS_Check_Data(&huart1, Pos11,Pos12,Pos21,Pos22);
}

void GPS_Check_Data(UART_HandleTypeDef* huart1, uint8_t Pos11, uint8_t Pos12, uint8_t Pos21, uint8_t Pos22)
{
	//Check if old position is the same as new(+-5)
	if((Pos11 < Pos21-5 || Pos11 > Pos21+5)
			&& (Pos12 < Pos22-5 || Pos12 > Pos22+5))
	{
		//Start timer 4.
		HAL_TIM_Base_Start_IT(&htim4);
	}
	else
	{
		// If device was in Standby mode
		if (Sleep == 1)
		{
			//Wake it up and start receiving data normally
 			Size = sprintf(Data,"$PMTK225,0*2B<CR><LF>");
			Sleep = 0;
			HAL_UART_Transmit(huart1, Data, Size,100);
			HAL_UART_Receive_IT(huart1, Received, 40);
		}
		Pos21 = Pos11;
		Pos22 = Pos12;
	}
}

void GPS_Go_Sleep(UART_HandleTypeDef* huart1)
{
	// Stop Timer 4. and reset cnt variable
	HAL_TIM_Base_Stop_IT(&htim4);
	Cnt = 0;
	//Go into Periodic Sleep Mode, 3 sec scanning, 2 min(!) sleep
	Size = sprintf(Data, "PMTK225,2,3000,120000*2B<CR><LF>");
	Sleep = 1;
	HAL_UART_Transmit(huart1, Data, Size,100);
}


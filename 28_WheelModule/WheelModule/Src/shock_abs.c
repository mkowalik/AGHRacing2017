
/*
 *
 ******************************************************************************
 * @file    shock_abs.c
 * @brief  	Shock absorber deflection sensor sensor routines.
 ******************************************************************************
 *  Created on: 03.06.2017
 *  Author: Nikodem Kastelik
 */

/* Includes ------------------------------------------------------------------*/
#include "shock_abs.h"
#include "adc.h"
/* Defines -------------------------------------------------------------------*/

#define SHOCK_ABS_APPROX_DEGREE 1		// degree of polynomial used for approximation
#define SHOCK_ABS_MAX_DEG	64			// maximum rotational position of sensor in degrees - from SAM simulation

#define shock_calib_lowval 620*1000			//must be known at compile time -  will vary on different modules
#define shock_calib_highval 1410*1000		//must be known at compile time -  will vary on different modules

/* Private Variables------------------------------------------------------------*/

int64_t measurement;	// variable for storing ADC register value and for computing it to micro degrees
const static int32_t shock_abs_coeff[SHOCK_ABS_APPROX_DEGREE+1]={3122,3437};	//polynomial coefficents (multiplied by 1000)

int32_t scale;

/**
  * @brief  USART2 communication for testing purposes
  * @param  None
  * @retval None
  * NOTE: Will be removed
  *
  */

void usart2_send8bit(uint8_t byte)
{
	USART2->TDR = byte;
	while(!(USART2->ISR & 0x40)); //wait till it is sent
}

void usart2_write_number(uint32_t number)
{
	char arr[10];
	uint32_t tmp=number;
	uint8_t length=0;
	int8_t i=0;

	if(tmp==0) length=1;
	else
	{
		while(tmp)	// loop to find number of digits of which consist 'number'
		{
			tmp/=10;
			length++;
		}
	}

	arr[length]='\0';	//mark the end of recently created array containing digits of 'number'

	for(i=length-1 ; i>-1 ; i--)	//fill array with digits of 'number'
	{
		arr[i]='0'+number%10;
		number/=10;
	}

	i=0;
	while(arr[i]!='\0')
	{
		usart2_send8bit(arr[i]);// sending data on LCD byte by byte
		i++;
	}

	usart2_send8bit('\n'); //send newline character
	usart2_send8bit('\r'); //send carriage return character
}



/**
  * @brief  Get Shock Absorber Deflection Sensor measurement from ADC and send it over CAN
  * @param  None
  * @retval None
  * NOTE: Lacks CAN functionality, Right now (temporary) sending over USART2
  */

void GetShockDeflection()
{
	//int32_t shock_calib_lowval=620*1000;			//must be known at compile time -  will vary on different modules
	//int32_t shock_calib_highval=1410*1000;		//must be known at compile time -  will vary on different modules

	scale = (shock_calib_highval-shock_calib_lowval)/SHOCK_ABS_MAX_DEG;
	measurement = (int64_t)ADC_GetConvResult(SHOCK_ABS_INDEX); //get adc result
	measurement = (1000000*measurement-shock_abs_coeff[1]*scale-1000*shock_calib_lowval)/(shock_abs_coeff[0]*scale/1000);
	//usart2_write_number((uint32_t)measurement);
}
//TODO send measurement over CAN




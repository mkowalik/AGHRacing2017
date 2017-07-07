/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "can.h"
#include "rtc.h"
#include "spi.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "string.h"
#include "ws2812_middleware.h"
#include "rpm_leds_driver.h"
#include "alert_leds_driver.h"
#include "fixed_point.h"
#include "gear_display_driver.h"
#include "actual_data_provider.h"
#include "can_receiver_driver.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

// checks if time1 is less than time2 for no more than maxDiffSeconds seconds
uint8_t timeCompare(RTC_TimeTypeDef time1, RTC_TimeTypeDef time2, uint32_t maxDiffSeconds){

	  uint32_t time1Seconds = time1.Hours*60*60 + time1.Minutes*60 + time1.Seconds;
	  uint32_t time2Seconds = time2.Hours*60*60 + time2.Minutes*60 + time2.Seconds;

	  if (time1Seconds + maxDiffSeconds >= time2Seconds)
		  return 1;
	  else
		  return 0;
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

volatile RTC_TimeTypeDef actualTime;
volatile RTC_DateTypeDef dummyDate;

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_SPI1_Init();
  MX_RTC_Init();

  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LED_DEBUG2_GPIO_Port, LED_DEBUG2_Pin);

  WS2812_Middleware_init();

  AlertLeds_Driver_init();
  RPMLeds_Driver_init();

  GearDisplay_Driver_init();

  ActualDataProvider_init();
  actualTime.Hours = 0;
  actualTime.Minutes = 0;
  actualTime.Seconds = 0;
  actualTime.TimeFormat = RTC_HOURFORMAT12_AM;
  actualTime.SubSeconds = 0;
  actualTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;

  HAL_RTC_SetTime(&hrtc, (RTC_TimeTypeDef*)&actualTime, RTC_FORMAT_BIN);
  HAL_RTCStateTypeDef srtc = HAL_RTC_GetState(&hrtc);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

	  HAL_RTC_GetTime(&hrtc, (RTC_TimeTypeDef*)&actualTime, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, (RTC_DateTypeDef*)&dummyDate, RTC_FORMAT_BIN);

	  ActualDataProvider_thread();


	  if (timeCompare(ActualDataProvider_getDataArrivalTime(RPM_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  //assuming no divider, multiplier, offset
		  RPMLeds_Driver_displayRPM(ActualDataProvider_getValue(RPM_DATA_CHANNEL));
	  } else {
		  RPMLeds_Driver_offDisplay();
	  }

	  if (timeCompare(ActualDataProvider_getDataArrivalTime(GEAR_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  //assuming no divider, multiplier, offset
		  GearDisplay_Driver_displayGear(ActualDataProvider_getValue(GEAR_DATA_CHANNEL), ActualDataProvider_getValue(GEAR_DATA_CHANNEL)==0);
	  } else {
		  GearDisplay_Driver_offDisplay();
	  }

	  if (timeCompare(ActualDataProvider_getDataArrivalTime(CLT_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  AlertLeds_Driver_displayCLT(FixedPoint_constr(
				  (int16_t)ActualDataProvider_getValue(CLT_DATA_CHANNEL),
				  ActualDataProvider_getDivider(CLT_DATA_CHANNEL),
				  ActualDataProvider_getMultiplier(CLT_DATA_CHANNEL),
				  ActualDataProvider_getOffset(CLT_DATA_CHANNEL),
				  FRACTIONAL_BITS_NUMBER));
	  } else {
		  AlertLeds_Driver_offCLT();
	  }

	  if (timeCompare(ActualDataProvider_getDataArrivalTime(BATT_VOLTAGE_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  AlertLeds_Driver_displayBatt(FixedPoint_constr(
				  ActualDataProvider_getValue(BATT_VOLTAGE_DATA_CHANNEL),
				  ActualDataProvider_getDivider(BATT_VOLTAGE_DATA_CHANNEL),
				  ActualDataProvider_getMultiplier(BATT_VOLTAGE_DATA_CHANNEL),
				  ActualDataProvider_getOffset(BATT_VOLTAGE_DATA_CHANNEL),
				  FRACTIONAL_BITS_NUMBER));
	  } else {
		  AlertLeds_Driver_offBatt();
	  }

	  if (timeCompare(ActualDataProvider_getDataArrivalTime(FUEL_LEVEL_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  AlertLeds_Driver_displayFuel(FixedPoint_constr(
				  ActualDataProvider_getValue(FUEL_LEVEL_DATA_CHANNEL),
				  ActualDataProvider_getDivider(FUEL_LEVEL_DATA_CHANNEL),
				  ActualDataProvider_getMultiplier(FUEL_LEVEL_DATA_CHANNEL),
				  ActualDataProvider_getOffset(FUEL_LEVEL_DATA_CHANNEL),
				  FRACTIONAL_BITS_NUMBER));
	  } else {
		  AlertLeds_Driver_offFuel();
	  }

	  if (timeCompare(ActualDataProvider_getDataArrivalTime(OIL_PRESSURE_DATA_CHANNEL), actualTime, CHANNEL_TIMEOUT) == 1){
		  AlertLeds_Driver_displayOilPres(FixedPoint_constr(
				  ActualDataProvider_getValue(OIL_PRESSURE_DATA_CHANNEL),
				  ActualDataProvider_getDivider(OIL_PRESSURE_DATA_CHANNEL),
				  ActualDataProvider_getMultiplier(OIL_PRESSURE_DATA_CHANNEL),
				  ActualDataProvider_getOffset(OIL_PRESSURE_DATA_CHANNEL),
				  FRACTIONAL_BITS_NUMBER));
	  } else {
		  AlertLeds_Driver_offOilPres();
	  }

	  HAL_Delay(50);

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void _Warning_Handler(char * file, int line)
{
	//TODO implementation
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

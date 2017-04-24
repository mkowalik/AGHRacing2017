/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
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
#include "spi.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "string.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

const uint8_t WS2812_reset[] = {
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000};

const uint8_t WS2812_off[] = {
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100};

const uint8_t WS2812_red[] = {
	0b10010010, 0b01001001, 0b00100100,
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100};

const uint8_t WS2812_blue[] = {
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100,
	0b11011011, 0b01101101, 0b10110110};

const uint8_t WS2812_green[] = {
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100};

const uint8_t WS2812_yellow[] = {
	0b11011011, 0b01101101, 0b10110110, //YELLOW
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100};

const uint8_t WS2812_test[] = {
	0b10101010, 0b10101010, 0b10101010, //YELLOW
	0b10101010, 0b10101010, 0b10101010,
	0b10101010, 0b10101010, 0b10101010};

uint8_t tab[9*9];

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LED_DEBUG2_GPIO_Port, LED_DEBUG2_Pin);

  for (uint8_t i=0; i<9; i++){
	  memcpy(tab+(i*9), WS2812_reset, 9);
  }
  HAL_SPI_Transmit(&hspi1, tab, 9*9, 100);

  uint8_t n = 1;
  uint8_t up = 1;
  uint8_t m = 0;
  uint8_t k = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  To jest jakaœ nasza wa¿na zmiana.

  const uint8_t* colors[] = {WS2812_red, WS2812_blue, WS2812_green, WS2812_yellow, WS2812_off};
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
//  for (uint8_t n=0; n<8; n++){

	  HAL_GPIO_WritePin(SlaveSelect_0_GPIO_Port, SlaveSelect_0_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(SlaveSelect_1_GPIO_Port, SlaveSelect_1_Pin, GPIO_PIN_RESET);

	  for (uint8_t i=0; i<9; i++){
//		  memcpy(tab+(i*9), WS2812_test, 9);
		  if (i<n){
			  memcpy(tab+(i*9), colors[k%4], 9);
		  } else {
			  memcpy(tab+(i*9), WS2812_off, 9);
		  }
	  }

	  HAL_SPI_Transmit(&hspi1, tab, 9*9, 200);

	  if (n==9) up=0;
	  if (n==0) {
		  up=1;
		  k++;
	  }

	  if (up){
		  n++;
	  } else {
		  n--;
	  }

	  HAL_GPIO_WritePin(SlaveSelect_0_GPIO_Port, SlaveSelect_0_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(SlaveSelect_1_GPIO_Port, SlaveSelect_1_Pin, GPIO_PIN_RESET);

	  for (uint8_t i=0; i<4; i++){
		  memcpy(tab+(i*9), colors[(m+i)%5], 9);
	  }

	  HAL_SPI_Transmit(&hspi1, tab, 4*9, 200);

	  m++;


	  HAL_GPIO_TogglePin(LED_DEBUG1_GPIO_Port, LED_DEBUG1_Pin);
	  HAL_GPIO_TogglePin(LED_DEBUG2_GPIO_Port, LED_DEBUG2_Pin);
	  HAL_Delay(100);

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
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
    Error_Handler();
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
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

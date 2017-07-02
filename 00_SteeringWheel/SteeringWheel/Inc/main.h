/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LCD_D5_Pin GPIO_PIN_13
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_14
#define LCD_D6_GPIO_Port GPIOC
#define ENC_B_Pin GPIO_PIN_15
#define ENC_B_GPIO_Port GPIOC
#define ENC_A_Pin GPIO_PIN_0
#define ENC_A_GPIO_Port GPIOA
#define V_BAT_ADC_Pin GPIO_PIN_1
#define V_BAT_ADC_GPIO_Port GPIOA
#define ENC_SW_Pin GPIO_PIN_2
#define ENC_SW_GPIO_Port GPIOA
#define SW_L3_Pin GPIO_PIN_3
#define SW_L3_GPIO_Port GPIOA
#define LCD_D7_Pin GPIO_PIN_4
#define LCD_D7_GPIO_Port GPIOA
#define V_LCD_ADC_Pin GPIO_PIN_5
#define V_LCD_ADC_GPIO_Port GPIOA
#define SW_L2_Pin GPIO_PIN_6
#define SW_L2_GPIO_Port GPIOA
#define SW_L1_Pin GPIO_PIN_7
#define SW_L1_GPIO_Port GPIOA
#define SW_R1_Pin GPIO_PIN_0
#define SW_R1_GPIO_Port GPIOB
#define SW_R2_Pin GPIO_PIN_1
#define SW_R2_GPIO_Port GPIOB
#define LCD_BM1_Pin GPIO_PIN_2
#define LCD_BM1_GPIO_Port GPIOB
#define LCD_D1_Pin GPIO_PIN_10
#define LCD_D1_GPIO_Port GPIOB
#define LCD_BM0_Pin GPIO_PIN_11
#define LCD_BM0_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOB
#define SW_R3_Pin GPIO_PIN_14
#define SW_R3_GPIO_Port GPIOB
#define LCD_D3_Pin GPIO_PIN_15
#define LCD_D3_GPIO_Port GPIOB
#define SW_LP_Pin GPIO_PIN_8
#define SW_LP_GPIO_Port GPIOA
#define LCD_CD_Pin GPIO_PIN_9
#define LCD_CD_GPIO_Port GPIOA
#define SW_RP_Pin GPIO_PIN_10
#define SW_RP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define LCD_WR0_Pin GPIO_PIN_15
#define LCD_WR0_GPIO_Port GPIOA
#define LCD_WR1_Pin GPIO_PIN_5
#define LCD_WR1_GPIO_Port GPIOB
#define LCD_D0_Pin GPIO_PIN_6
#define LCD_D0_GPIO_Port GPIOB
#define LCD_D2_Pin GPIO_PIN_7
#define LCD_D2_GPIO_Port GPIOB
#define LCD_K_PWM_Pin GPIO_PIN_8
#define LCD_K_PWM_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_9
#define LCD_D4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#ifdef __cplusplus
extern "C"
#endif

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(NULL, 0)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

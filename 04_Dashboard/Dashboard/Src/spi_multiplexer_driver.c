/*
 * spi_multiplexer_driver.c
 *
 *  Created on: 03.05.2017
 *      Author: kowalik
 */

#include "spi_multiplexer_driver.h"

#define SLAVE_SELECT_0_PORT SlaveSelect_0_GPIO_Port
#define SLAVE_SELECT_1_PORT SlaveSelect_1_GPIO_Port

#define SLAVE_SELECT_0_PIN SlaveSelect_0_Pin
#define SLAVE_SELECT_1_PIN SlaveSelect_1_Pin

void SPI_MuxDriver_TransmitData(uint8_t* pData, uint16_t size, uint8_t channel, GPIO_TypeDef* latchPort, uint16_t latchPin){

	if (latchPort != NULL){
		HAL_GPIO_WritePin(latchPort, latchPin, GPIO_PIN_SET);
	}

	GPIO_PinState SlaveSelect_0_State;
	GPIO_PinState SlaveSelect_1_State;

	switch (channel) {
		default:
		case 0:
			SlaveSelect_0_State = GPIO_PIN_RESET;
			SlaveSelect_1_State = GPIO_PIN_RESET;
			break;
		case 1:
			SlaveSelect_0_State = GPIO_PIN_SET;
			SlaveSelect_1_State = GPIO_PIN_RESET;
			break;
		case 2:
			SlaveSelect_0_State = GPIO_PIN_RESET;
			SlaveSelect_1_State = GPIO_PIN_SET;
			break;
		case 3:
			SlaveSelect_0_State = GPIO_PIN_SET;
			SlaveSelect_1_State = GPIO_PIN_SET;
			break;
	}


	HAL_GPIO_WritePin(SLAVE_SELECT_0_PORT, SLAVE_SELECT_0_PIN, SlaveSelect_0_State);
	HAL_GPIO_WritePin(SLAVE_SELECT_1_PORT, SLAVE_SELECT_1_PIN, SlaveSelect_1_State);

	HAL_SPI_Transmit(&hspi1, pData, size, 100);

	if (latchPort != NULL){
		HAL_GPIO_WritePin(latchPort, latchPin, GPIO_PIN_RESET);
	}

}

void SPI_MuxDriver_TransmitDataNoLatch(uint8_t* pData, uint16_t size, uint8_t channel){
	SPI_MuxDriver_TransmitData(pData, size, channel, NULL, 0);
}

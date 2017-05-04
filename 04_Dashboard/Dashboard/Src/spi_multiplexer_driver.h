/*
 * spi_multiplexer_driver.h
 *
 *  Created on: 03.05.2017
 *      Author: kowalik
 */

#ifndef SPI_MULTIPLEXER_DRIVER_H_
#define SPI_MULTIPLEXER_DRIVER_H_

#include "stm32f0xx_hal.h"
#include "spi.h"

void SPI_MuxDriver_TransmitData(uint8_t* pData, uint16_t size, uint8_t channel, GPIO_TypeDef* latchPort, uint16_t latchPin);
void SPI_MuxDriver_TransmitDataNoLatch(uint8_t* pData, uint16_t size, uint8_t channel);

#endif /* SPI_MULTIPLEXER_DRIVER_H_ */

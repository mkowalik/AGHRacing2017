#ifndef __NRF24_HAL_H
#define __NRF24_HAL_H

// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions

// Peripheral libraries
#include "stm32f0xx_hal.h"
#include "spi.h"
#include "main.h"

// SPI port peripheral
#define nRF24_SPI_HANDLE           &hspi1

// CE (chip enable) pin (PB11)
#define nRF24_CE_PORT              nRF24_CE_GPIO_Port
#define nRF24_CE_PIN               nRF24_CE_Pin
#define nRF24_CE_L()               ((nRF24_CE_PORT)->BRR = (nRF24_CE_PIN))
#define nRF24_CE_H()               ((nRF24_CE_PORT)->BSRR = (nRF24_CE_PIN))

// CSN (chip select negative) pin (PB12)
#define nRF24_CSN_PORT             nRF24_CSN_GPIO_Port
#define nRF24_CSN_PIN              nRF24_CSN_Pin
#define nRF24_CSN_L()              ((nRF24_CSN_PORT)->BRR = (nRF24_CSN_PIN))
#define nRF24_CSN_H()              ((nRF24_CSN_PORT)->BSRR = (nRF24_CSN_PIN))

// Function prototypes
uint8_t nRF24_LL_RW(uint8_t data);

#endif // __NRF24_HAL_H

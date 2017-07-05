#ifndef __NRF24_HAL_H
#define __NRF24_HAL_H
#ifdef __cplusplus
 extern "C" {
#endif

// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions

// Peripheral libraries
#include "stm32f0xx_hal.h"
#include "spi.h"
#include "main.h"

#define NRF24_DEVICES_NUMBER	1

typedef struct nRF24_Device{
	SPI_HandleTypeDef * SPIHandle;
	GPIO_TypeDef * CE_GPIO_Port;
	uint16_t CE_Pin;
	GPIO_TypeDef * CSN_GPIO_Port;
	uint16_t CSN_Pin;
	GPIO_TypeDef * IRQ_GPIO_Port;
	uint16_t IRQ_Pin;
	int8_t InterruptNumber;
} nRF24_Device_t;

nRF24_Device_t nRF24Devices[NRF24_DEVICES_NUMBER];

#define nRF24_CE_L(__device)               ((__device->CE_GPIO_Port)->BRR = (__device->CE_Pin))
#define nRF24_CE_H(__device)               ((__device->CE_GPIO_Port)->BSRR = (__device->CE_Pin))
#define nRF24_CSN_L(__device)              ((__device->CSN_GPIO_Port)->BRR = (__device->CSN_Pin))
#define nRF24_CSN_H(__device)              ((__device->CSN_GPIO_Port)->BSRR = (__device->CSN_Pin))
#define nRF24_ENA_IRQ(__device)				(HAL_NVIC_EnableIRQ((IRQn_Type) __device->InterruptNumber))
#define nRF24_DIS_IRQ(__device)				(HAL_NVIC_DisableIRQ((IRQn_Type) __device->InterruptNumber))

// Function prototypes
uint8_t nRF24_LL_RW(nRF24_Device_t * device, uint8_t data);
void nRF24_DevicesInit(void);

#ifdef __cplusplus
}
#endif
#endif // __NRF24_HAL_H

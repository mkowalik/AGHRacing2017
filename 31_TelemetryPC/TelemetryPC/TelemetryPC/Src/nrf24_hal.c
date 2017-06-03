#include "nrf24_hal.h"

// Low level SPI transmit/receive function (hardware depended)
// input:
//   data - value to transmit via SPI
// return: value received from SPI
uint8_t nRF24_LL_RW(nRF24_Device_t * device, uint8_t data) {
	uint8_t RxData;
	HAL_SPI_TransmitReceive(device->SPIHandle, &data, &RxData, 1, 100);
	return RxData;
}

void nRF24_DevicesInit(void){

	/* Initialise each device */
	nRF24Devices[0].SPIHandle = &hspi1;
	nRF24Devices[0].CE_GPIO_Port = nRF24_CE_GPIO_Port;
	nRF24Devices[0].CE_Pin = nRF24_CE_Pin;
	nRF24Devices[0].CE_GPIO_Port = nRF24_CSN_GPIO_Port;
	nRF24Devices[0].CE_Pin = nRF24_CSN_Pin;
	nRF24Devices[0].CE_GPIO_Port = nRF24_IRQ_GPIO_Port;
	nRF24Devices[0].CE_Pin = nRF24_IRQ_Pin;
	nRF24Devices[0].InterruptNumber = EXTI4_15_IRQn;
}

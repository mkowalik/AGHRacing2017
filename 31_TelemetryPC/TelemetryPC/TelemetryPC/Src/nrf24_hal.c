#include "nrf24_hal.h"

// Configure the GPIO lines of the nRF24L01 transceiver
// note: IRQ pin must be configured separately
void nRF24_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

    // Enable the nRF24L01 GPIO peripherals
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure CSN pin */
	GPIO_InitStruct.Pin = nRF24_CSN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(nRF24_CSN_PORT, &GPIO_InitStruct);

	/*Configure CE pin */
	GPIO_InitStruct.Pin = nRF24_CE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(nRF24_CE_PORT, &GPIO_InitStruct);

	/* CSN high = disable SPI */
	nRF24_CSN_H();

	/* CE low = disable TX/RX */
	nRF24_CE_L();
}

// Low level SPI transmit/receive function (hardware depended)
// input:
//   data - value to transmit via SPI
// return: value received from SPI
uint8_t nRF24_LL_RW(uint8_t data) {
	uint8_t RxData;
	HAL_SPI_TransmitReceive(nRF24_SPI_HANDLE, &data, &RxData, 1, 100);
	return RxData;
}

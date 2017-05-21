#include "nrf24_hal.h"

// Low level SPI transmit/receive function (hardware depended)
// input:
//   data - value to transmit via SPI
// return: value received from SPI
uint8_t nRF24_LL_RW(uint8_t data) {
	uint8_t RxData;
	HAL_SPI_TransmitReceive(nRF24_SPI_HANDLE, &data, &RxData, 1, 100);
	return RxData;
}

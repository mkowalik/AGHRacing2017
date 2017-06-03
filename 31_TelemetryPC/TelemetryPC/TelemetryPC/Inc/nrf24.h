#ifndef __NRF24_H
#define __NRF24_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "nrf24_hal.h"
#include "nRF24L01p.h"

/**
  * @brief  nRF24 Status structures definition
  */
typedef enum
{
  nRF24_OK       = 0x00,
  nRF24_ERROR    = 0x01,
  nRF24_BUSY     = 0x02,
  nRF24_TIMEOUT  = 0x03
} nRF24_StatusTypeDef;

// Function prototypes
nRF24_StatusTypeDef nRF24_Init(nRF24_Device_t * device, uint32_t timeout);
uint8_t nRF24_Check(nRF24_Device_t * device);

void nRF24_SetPowerMode(nRF24_Device_t * device, uint8_t mode);
void nRF24_SetOperationalMode(nRF24_Device_t * device, uint8_t mode);
nRF24_StatusTypeDef nRF24_SetRFChannel(nRF24_Device_t * device, uint8_t channel);
nRF24_StatusTypeDef nRF24_SetAutoRetr(nRF24_Device_t * device, nRF24_ardDelay_t ard, uint8_t arc);
nRF24_StatusTypeDef nRF24_SetAddrWidth(nRF24_Device_t * device, uint8_t addrWidth);
nRF24_StatusTypeDef nRF24_SetAddr(nRF24_Device_t * device, nRF24_pipe_t pipe, const uint8_t *addr);
void nRF24_SetTXPower(nRF24_Device_t * device, uint8_t tx_pwr);
void nRF24_SetDataRate(nRF24_Device_t * device, uint8_t data_rate);
void nRF24_SetCRCScheme(nRF24_Device_t * device, uint8_t scheme);
nRF24_StatusTypeDef nRF24_SetRXPipe(nRF24_Device_t * device, nRF24_pipe_t pipe, nRF24_autoAcknowledgment_t aaState, uint8_t payloadLen);
nRF24_StatusTypeDef nRF24_ClosePipe(nRF24_Device_t * device, nRF24_pipe_t pipe);
nRF24_StatusTypeDef nRF24_EnableAA(nRF24_Device_t * device, uint8_t pipe);
void nRF24_DisableAA(nRF24_Device_t * device, uint8_t pipe);

uint8_t nRF24_GetStatus(nRF24_Device_t * device);
uint8_t nRF24_GetIRQFlags(nRF24_Device_t * device);
uint8_t nRF24_GetStatus_RXFIFO(nRF24_Device_t * device);
uint8_t nRF24_GetStatus_TXFIFO(nRF24_Device_t * device);
uint8_t nRF24_GetRXSource(nRF24_Device_t * device);
uint8_t nRF24_GetRetransmitCounters(nRF24_Device_t * device);

void nRF24_ResetPLOS(nRF24_Device_t * device);
void nRF24_FlushTX(nRF24_Device_t * device);
void nRF24_FlushRX(nRF24_Device_t * device);
void nRF24_ClearIRQFlags(nRF24_Device_t * device);

void nRF24_WritePayload(nRF24_Device_t * device, uint8_t *pBuf, uint8_t length);
nRF24_RXResult nRF24_ReadPayload(nRF24_Device_t * device, uint8_t *pBuf, uint8_t *length);
void nRF24_TransmitPacketIRQ(nRF24_Device_t * device, uint8_t *pBuf, uint8_t length);

#ifdef __cplusplus
}
#endif
#endif // __NRF24_H

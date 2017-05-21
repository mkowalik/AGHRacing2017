#ifndef __NRF24_H
#define __NRF24_H

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
nRF24_StatusTypeDef nRF24_Init(uint32_t timeout);
uint8_t nRF24_Check(void);

void nRF24_SetPowerMode(uint8_t mode);
void nRF24_SetOperationalMode(uint8_t mode);
nRF24_StatusTypeDef nRF24_SetRFChannel(uint8_t channel);
nRF24_StatusTypeDef nRF24_SetAutoRetr(nRF24_ardDelay_t ard, uint8_t arc);
nRF24_StatusTypeDef nRF24_SetAddrWidth(uint8_t addrWidth);
nRF24_StatusTypeDef nRF24_SetAddr(nRF24_pipe_t pipe, const uint8_t *addr);
void nRF24_SetTXPower(uint8_t tx_pwr);
void nRF24_SetDataRate(uint8_t data_rate);
void nRF24_SetCRCScheme(uint8_t scheme);
nRF24_StatusTypeDef nRF24_SetRXPipe(nRF24_pipe_t pipe, nRF24_autoAcknowledgment_t aaState, uint8_t payloadLen);
nRF24_StatusTypeDef nRF24_ClosePipe(nRF24_pipe_t pipe);
nRF24_StatusTypeDef nRF24_EnableAA(uint8_t pipe);
void nRF24_DisableAA(uint8_t pipe);

uint8_t nRF24_GetStatus(void);
uint8_t nRF24_GetIRQFlags(void);
uint8_t nRF24_GetStatus_RXFIFO(void);
uint8_t nRF24_GetStatus_TXFIFO(void);
uint8_t nRF24_GetRXSource(void);
uint8_t nRF24_GetRetransmitCounters(void);

void nRF24_ResetPLOS(void);
void nRF24_FlushTX(void);
void nRF24_FlushRX(void);
void nRF24_ClearIRQFlags(void);

void nRF24_WritePayload(uint8_t *pBuf, uint8_t length);
nRF24_RXResult nRF24_ReadPayload(uint8_t *pBuf, uint8_t *length);
void nRF24_TransmitPacketIRQ(uint8_t *pBuf, uint8_t length);


void nRF24_SetUpRx(uint8_t channel, nRF24_DataRate_t nRF24DataRate, uint8_t nRF24_ADDR[], uint8_t addrLength, uint8_t payloadLenght);
void nRF24_SetUpTx(uint8_t channel, nRF24_DataRate_t nRF24DataRate, uint8_t nRF24_ADDR[], uint8_t addrLength, uint8_t payloadLenght);

#define nRF24_RX_ON()   nRF24_CE_H();
#define nRF24_RX_OFF()  nRF24_CE_L();


#endif // __NRF24_H

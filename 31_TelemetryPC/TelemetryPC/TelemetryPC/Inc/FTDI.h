/*
 * FTDI.h
 *
 *  Created on: May 21, 2017
 *      Author: Wojciech
 */

#ifndef FTDI_H_
#define FTDI_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "network.h"
#include "fifo.h"

#define FTDI_MESSAGE_SIZE		32

typedef enum{
  FTDI_OK       = 0x00,
  FTDI_ERROR    = 0x01,
  FTDI_BUSY     = 0x02,
  FTDI_TIMEOUT  = 0x03
} FTDI_StatusTypeDef;

typedef struct{
	uint8_t data[FTDI_MESSAGE_SIZE];
} FTDI_frame_t;

typedef struct{
	uint8_t data[31];
} FTDI_data_t;

extern volatile Fifo_Handle_t ftdiTxFifo;
extern volatile Fifo_Handle_t ftdiRxFifo;

void FTDI_Init(void);
FTDI_StatusTypeDef FTDI_Receive(uint8_t * data, uint8_t * dataLen);
FTDI_StatusTypeDef FTDI_Send(uint8_t * data, uint8_t dataLen);

#ifdef __cplusplus
}
#endif
#endif /* FTDI_H_ */

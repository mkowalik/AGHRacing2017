/*
 * FTDI.h
 *
 *  Created on: May 21, 2017
 *      Author: Wojciech
 */

#ifndef FTDI_H_
#define FTDI_H_

#include "network.h"
#include "fifo.h"

#define MESSAGE_SIZE		32

typedef enum{
  FTDI_OK       = 0x00,
  FTDI_ERROR    = 0x01,
  FTDI_BUSY     = 0x02,
  FTDI_TIMEOUT  = 0x03
} FTDI_StatusTypeDef;

typedef struct{
	uint8_t range			: 2;
	uint8_t datapresent		: 1;
	uint8_t speed			: 3;
	uint8_t reserved1		: 2;
} FTDI_settings_t;

typedef struct{
	uint8_t data[31];
} FTDI_data_t;

typedef struct{
	FTDI_data_t			data;
	FTDI_settings_t 	settings;
} FTDI_payload_t;

extern volatile Fifo_Handle_t ftdiTxFifo;
extern volatile Fifo_Handle_t ftdiRxFifo;

void FTDI_Init(void);
FTDI_StatusTypeDef FTDI_Receive(uint8_t * data, uint8_t * dataLen);
FTDI_StatusTypeDef FTDI_Send(uint8_t * data, uint8_t dataLen);

#endif /* FTDI_H_ */

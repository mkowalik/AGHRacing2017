/*
 * FTDI.c
 *
 *  Created on: May 21, 2017
 *      Author: Wojciech
 */

#include "FTDI.h"

#define FTDI_FIFO_SIZE		100

static volatile FTDI_data_t ftdiTxFifoArray[FTDI_FIFO_SIZE];
static volatile FTDI_data_t ftdiRxFifoArray[FTDI_FIFO_SIZE];

volatile Fifo_Handle_t ftdiTxFifo;
volatile Fifo_Handle_t ftdiRxFifo;

void FTDI_Init(void){
	ftdiTxFifo = Fifo_Init( (void *) ftdiTxFifoArray, sizeof(FTDI_data_t), FTDI_FIFO_SIZE);
	ftdiRxFifo = Fifo_Init( (void *) ftdiRxFifoArray, sizeof(FTDI_data_t), FTDI_FIFO_SIZE);
}

FTDI_StatusTypeDef FTDI_Receive(uint8_t * data, uint8_t * dataLen){
	if(FIFO_OK == Fifo_PullElement( (Fifo_Handle_t *) &ftdiRxFifo, data)){
		*dataLen = ftdiRxFifo.uxItemSize;
		return FTDI_OK;
	}
	*dataLen = 0;
	return FTDI_ERROR;
}

FTDI_StatusTypeDef FTDI_Send(uint8_t * data, uint8_t dataLen){
	if(dataLen == ftdiTxFifo.uxItemSize){
		if(FIFO_OK == Fifo_PushElement( (Fifo_Handle_t *) &ftdiTxFifo, data)){
			return FTDI_OK;
		}
		return FTDI_ERROR;
	}
	return FTDI_ERROR;
}

/*
 * os.h
 *
 *  Created on: Apr 21, 2017
 *      Author: Wojciech
 */

#ifndef OS_H_
#define OS_H_

#include "nrf24.h"
#include <fifo.h>

/* SYSTEM CONFIGURATION */
#define PRIMARY_MODE	nRF24_MODE_TX
#define MESSAGE_SIZE	31

typedef enum {Range_0 = 0, Range_1 = 1, Range_2 = 2, Range_3 = 3} wirelessRange_t;

extern nRF24_mode_t transceiverMode;
extern nRF24_Speed_t nRF24Speed;
extern wirelessRange_t wirelessRange;

void Tele_Init(volatile fifo_t * periphTxFifo, volatile fifo_t * periphRxFifo);
void Tele_Handler(void);
void nFR24_InterruptHandler(void);

#endif /* OS_H_ */

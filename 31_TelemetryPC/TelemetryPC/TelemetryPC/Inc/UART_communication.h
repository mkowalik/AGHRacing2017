/*
 * UART_communication.h
 *
 *  Created on: Apr 11, 2017
 *      Author: Wojciech
 */

#ifndef UART_COMMUNICATION_H_
#define UART_COMMUNICATION_H_

#include "stdio.h"
#include "network.h"

/* USER DEFINES */
#define UART_Handle		(&huart4)
#define TERMINATOR 		'\n'
#define UART_RECEIVER_SIZE 		50
#define UART_TRANSMITTER_SIZE	50
/*END OF USER DEFINES */

typedef enum {
	EMPTY,
	READY,
	OVERFLOW
} receiverStatus_t;

typedef enum {
	FREE,
	BUSY
} rransmitterStatus_t;

void UART_SendBufHex(uint8_t * arrayToSend, uint16_t arrayLength);
void UART_SendStr(uint8_t * stringToSend);
void UART_SendInt(int32_t intToSend);

uint8_t UART_StartTx_IT(uint8_t * pData);
uint8_t UART_StartRx_IT(uint8_t * pData);

uint8_t UART_Receive(uint8_t * receivedData, uint16_t length);
void ReceiverBuffer_GetStringCopy(uint8_t *destination);
receiverStatus_t ReceiverBuffer_GetStatus(void);
void ReceiverBuffer_PutCharacterToBuffer(uint8_t newChar);
void ReceiverBuffer_Clear(void);

#endif /* UART_COMMUNICATION_H_ */

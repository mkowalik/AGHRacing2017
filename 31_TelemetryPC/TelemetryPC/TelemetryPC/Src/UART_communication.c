/*
 * UART_communication.c
 *
 *  Created on: Apr 11, 2017
 *      Author: Wojciech
 */

#include "UART_communication.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define NULL_SIGN			(0)
#define MAX_COMMAND_SIZE	8

typedef struct{
	uint8_t data[UART_RECEIVER_SIZE];
	uint8_t charCounter;
	receiverStatus_t status;
} receiverBuffer_t;

receiverBuffer_t receiverBuffer;

static void UART_Transmit(uint8_t * dataToSend, uint16_t length);

void ReceiverBuffer_PutCharacterToBuffer(uint8_t cChar);

/* HAL communication functions */
static void UART_Transmit(uint8_t * dataToSend, uint16_t length){
	HAL_UART_Transmit(UART_Handle, (uint8_t *) dataToSend, length, length*10);
}

uint8_t UART_Receive(uint8_t * receivedData, uint16_t length){
	if(HAL_OK == HAL_UART_Receive(UART_Handle, (uint8_t *) receivedData, length, length*100)){
		return 1;
	}
	return 0;
}

uint8_t UART_StartTx_IT(uint8_t * pData){
	if(HAL_UART_Transmit_IT(UART_Handle, pData, MESSAGE_SIZE) == HAL_OK){
		return 1;
	}
	return 0;
}

uint8_t UART_StartRx_IT(uint8_t * pData){
	if(HAL_UART_Receive_IT(UART_Handle, pData, MESSAGE_SIZE) == HAL_OK){
		return 1;
	}
	return 0;
}

void UART_SendBufHex(uint8_t * arrayToSend, uint16_t arrayLength){
	uint8_t buffString[20];

	for(uint16_t hexCounter=0; hexCounter < arrayLength; hexCounter++){
		sprintf((char *) buffString, " 0x%x", arrayToSend[hexCounter]);
		UART_Transmit(buffString, strlen((char *) buffString));
	}
}

void UART_SendStr(uint8_t * stringToSend){
	UART_Transmit(stringToSend, strlen((char *)stringToSend));
}

void UART_SendInt(int32_t intToSend){
	uint8_t str[15];

	sprintf((char *) str, "%ld", intToSend);
	UART_Transmit(str, strlen((char *) str));
}

void ReceiverBuffer_PutCharacterToBuffer(uint8_t newChar){
	if(receiverBuffer.charCounter == UART_RECEIVER_SIZE){
		receiverBuffer.status = OVERFLOW;
	}
	else if(newChar == TERMINATOR){
		receiverBuffer.data[receiverBuffer.charCounter] = NULL_SIGN;
		receiverBuffer.status = READY;
	}
	else{
		receiverBuffer.data[receiverBuffer.charCounter] = newChar;
		receiverBuffer.charCounter++;
		receiverBuffer.status = EMPTY;
	}
}

receiverStatus_t ReceiverBuffer_GetStatus(void){
	return receiverBuffer.status;
}

void ReceiverBuffer_GetStringCopy(uint8_t *destination){
	strcpy((char *) destination, (char *) receiverBuffer.data);
	receiverBuffer.charCounter = 0;
	receiverBuffer.status = EMPTY;
}

void ReceiverBuffer_Clear(void){
	receiverBuffer.data[0] = 0;
	receiverBuffer.charCounter = 0;
	receiverBuffer.status = EMPTY;
}

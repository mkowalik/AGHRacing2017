/*
 * usb.c
 *
 *  Created on: May 3, 2017
 *      Author: Wojciech
 */
#include <fifo.h>
#include "decoder.h"
#include "UART_communication.h"
#include "usart.h"

static uint8_t UART_payload_Tx[32];
static uint8_t UART_payload_Rx[32];

void USB_EstablishConnection(void){
	uint8_t cReceivedMessage[UART_RECEIVER_SIZE];
	int8_t connectionEstablished = 0;
	uint8_t connectedMsg[] = "CONNECTED";
	uint8_t recChar;

	do{
		if(UART_Receive(&recChar, 1)){
			if(recChar != 0){
				ReceiverBuffer_PutCharacterToBuffer(recChar);
			}
		}
		if(ReceiverBuffer_GetStatus() == READY){
			ReceiverBuffer_GetStringCopy(cReceivedMessage);
			ReplaceCharactersInString(cReceivedMessage, TERMINATOR, NULL_SIGN);
			DecodeMsg(cReceivedMessage);

			if((tokenNumber != 0) && (token[0].type == KEYWORD)){
				switch(token[0].value.keyword){

					case CONNECT:
						UART_SendStr(connectedMsg);
						connectionEstablished = 1;
						break;

				}
			}

		}
		else if(ReceiverBuffer_GetStatus() == OVERFLOW){
			ReceiverBuffer_Clear();
		}

	} while(connectionEstablished == 0);
	HAL_Delay(200);
	UART_StartTx_IT((uint8_t *) &UART_payload_Tx);
	UART_StartRx_IT((uint8_t *) &UART_payload_Rx);
}

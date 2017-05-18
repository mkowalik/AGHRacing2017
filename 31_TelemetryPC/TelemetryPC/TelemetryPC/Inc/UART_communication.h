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
#define MESSAGE_SIZE		32
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

typedef union{
	struct message{
		struct header{
			uint8_t reserved	: 3;
			uint8_t range		: 2;
			uint8_t control 	: 1;
			uint8_t canFrames	: 2;
		} header;

		union data{
			struct can{
				uint8_t IDl;
				uint8_t IDh			: 3;
				uint8_t dataLength	: 4;
				uint8_t reserved	: 1;
				uint8_t data[8];
			} can[3];

			struct control{
				uint8_t	txPermission: 1;
				uint8_t	filterNum	: 6;
				uint8_t reserved	: 1;
				uint8_t speed;
				struct filter{
				    uint8_t IDl;
				    uint8_t IDh			: 3;
				    uint8_t onReq		: 1;
				    uint8_t onState     : 1;
					uint8_t reserved	: 3;
				} filter[14];
			} control;

		} data;
	} message;

	uint8_t payload[31];
} message_t;

typedef struct{
	uint8_t range			: 2;
	uint8_t datapresent		: 1;
	uint8_t speed			: 3;
	uint8_t reserved1		: 2;
} UART_settings_t;

typedef struct{
	uint8_t data[31];
} UART_data_t;

typedef struct{
	UART_data_t		data;
	UART_settings_t 	settings;
} UART_payload_t;

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

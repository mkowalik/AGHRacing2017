/*
 * os.c
 *
 *  Created on: Apr 21, 2017
 *      Author: Wojciech
 */
#include <tele.h>
#include <fifo.h>
#include <usart.h>
#include "delay.h"

typedef enum {
	ReceiveMode,
	TransmitMode,
	ChangeMode,
	WaitForWirelessConnection
} smState_t;

typedef struct{
	union{
		struct{
			uint8_t packLostCnt : 4;
			uint8_t autoRetrCnt	: 4;
		} nibbles;

		uint8_t otx;
	} nRFInto;

	uint16_t packLostCnt;
} packetLostStat_t;

static volatile fifo_t nRFTxFifo;
static volatile fifo_t nRFRxFifo;

static message_t MyMessage;
static message_t * pMyMessage;
static const uint8_t nRF24_ADDR[] = { 'A', 'G', 'H' };

static volatile smState_t wlState;
static volatile packetLostStat_t packetLostStat;
static volatile nRF24_RXResult pipe;
static volatile uint8_t payload_length;
static volatile uint16_t receiverTxFifoSize;

nRF24_mode_t transceiverMode;
nRF24_Speed_t nRF24Speed = nRF24_DR_250kbps;;
wirelessRange_t wirelessRange = Range_0;

void Tele_SetUp(uint8_t channel, nRF24_Speed_t nRF24Speed, nRF24_mode_t transceiverMode, uint8_t nRF24_ADDR[], uint8_t addrLength, uint8_t payloadLenght);
uint8_t Tele_EstablishConnection(nRF24_mode_t transceiverMode, nRF24_Speed_t nRF24Speed, uint8_t payloadLength, uint32_t Timeout);
uint8_t Tele_SendTransmissionChangeFrame(nRF24_Speed_t nRF24Speed, uint8_t txPermission);
wirelessRange_t Tele_GetRange(void);
uint8_t Tele_CheckConnection(uint32_t Timeout);

uint8_t nRF24_SendMessage(message_t * Message);
uint8_t nRF24_ReceiveMessage(message_t * Message);
uint8_t PeriphSendMessage(message_t * Message);
uint8_t PeriphReceiveMessage(message_t * Message);

void Tele_Init(volatile fifo_t * periphTxFifo, volatile fifo_t * periphRxFifo){
	pMyMessage = &MyMessage;
	transceiverMode = PRIMARY_MODE;
	nRF24Speed = nRF24_DR_250kbps;
	payload_length = 31;
	wlState = WaitForWirelessConnection;

	Fifo_Init((fifo_t *) &nRFTxFifo);
	Fifo_Init((fifo_t *) &nRFRxFifo);
	Fifo_Init((fifo_t *) periphTxFifo);
	Fifo_Init((fifo_t *) periphRxFifo);

	nRF24_GPIO_Init();
	while(!nRF24_Check()){}
	nRF24_Init();
	Tele_SetUp(40, nRF24Speed, transceiverMode, (uint8_t *) nRF24_ADDR, 3, payload_length);
}

void Tele_Handler(void){

	switch(wlState){

	case WaitForWirelessConnection:
		wirelessRange = Range_0;
		if(Tele_EstablishConnection(transceiverMode, nRF24Speed, payload_length, 100)){
			if(transceiverMode == nRF24_MODE_RX){
				wlState = ReceiveMode;
			}
			else if(transceiverMode == nRF24_MODE_TX){
				wlState = TransmitMode;
			}
		}
		else{
			//Some banana happens during connection, try on default setup
			transceiverMode = PRIMARY_MODE;
			nRF24Speed = nRF24_DR_250kbps;
			nRF24_CE_L();
			nRF24_SetPowerMode(nRF24_PWR_DOWN);
			wlState = ChangeMode;
		}
		break;

	case ReceiveMode:
		if(Tele_CheckConnection(100) == 0){
			wlState = WaitForWirelessConnection;
		}
		if(nRF24_ReceiveMessage(pMyMessage)){
			PeriphSendMessage(pMyMessage);

			if(MyMessage.message.header.control){

				if(MyMessage.message.data.control.txPermission){
					transceiverMode = nRF24_MODE_TX;
					rxReceived = 0;
					nRF24_CE_L();
					while(!rxReceived){}
					nRF24_SetPowerMode(nRF24_PWR_DOWN);
					wlState = ChangeMode;
				}

				if(MyMessage.message.data.control.speed != nRF24Speed){
					nRF24Speed = MyMessage.message.data.control.speed;
					rxReceived = 0;
					nRF24_CE_L();
					while(!rxReceived){}
					nRF24_SetPowerMode(nRF24_PWR_DOWN);
					wlState = ChangeMode;
				}

			}
		}
		break;

	case TransmitMode:
		if(Tele_CheckConnection(100) == 0){
			wlState = WaitForWirelessConnection;
		}
		wirelessRange = Tele_GetRange();
		if(PeriphReceiveMessage(pMyMessage)){
			nRF24_SendMessage(pMyMessage);
		}

		if(receiverTxFifoSize > nRFTxFifo.elementsInFifo){
			if(Tele_SendTransmissionChangeFrame(nRF24Speed, 1)){
				transceiverMode = nRF24_MODE_RX;
				wlState = ChangeMode;
			}
		}

		if(HAL_GetTick() > 5000){
			if(nRF24Speed == nRF24_DR_250kbps){
				if(Tele_SendTransmissionChangeFrame(nRF24_DR_2Mbps, 0)){
					nRF24Speed = nRF24_DR_2Mbps;
					wlState = ChangeMode;
				}
			}
		}
		if(HAL_GetTick() > 10000){
			if(nRF24Speed == nRF24_DR_2Mbps){
				if(Tele_SendTransmissionChangeFrame(nRF24_DR_1Mbps, 0)){
					nRF24Speed = nRF24_DR_1Mbps;
					wlState = ChangeMode;
				}
			}
		}
//		else if(nRF24Speed == nRF24_DR_1Mbps){
//			if(Tele_SendTransmissionChangeFrame(nRF24_DR_2Mbps, 0)){
//				nRF24Speed = nRF24_DR_2Mbps;
//				wlState = ChangeMode;
//			}
//		}

		break;

	case ChangeMode:
		Tele_SetUp(40, nRF24Speed, transceiverMode, (uint8_t *) nRF24_ADDR, 3, payload_length);
		wlState = WaitForWirelessConnection;
		break;

	}
}

uint8_t Tele_CheckConnection(uint32_t Timeout){
	static uint32_t tickstart;

	switch(transceiverMode){
	case nRF24_MODE_RX:
		if(rxReceived){
			rxReceived = 0;
			tickstart = HAL_GetTick();
			return 1;
		}
		else if((HAL_GetTick()-tickstart) > Timeout){
			return 0;
		}
		break;

	case nRF24_MODE_TX:
		if(txReceived){
			txReceived = 0;
			tickstart = HAL_GetTick();
			return 1;
		}
		else if((HAL_GetTick()-tickstart) > Timeout){
			return 0;
		}
		break;
	}
	return 1;
}

wirelessRange_t Tele_GetRange(void){
	if(packetLostStat.nRFInto.nibbles.autoRetrCnt < 6){
		return Range_3;
	}
	else if(packetLostStat.nRFInto.nibbles.autoRetrCnt < 11){
		return Range_2;
	}
	else{
		return Range_1;
	}
}

void Tele_SetUp(uint8_t channel, nRF24_Speed_t nRF24Speed, nRF24_mode_t transceiverMode, uint8_t nRF24_ADDR[], uint8_t addrLength, uint8_t payloadLenght){
	nRF24_SetRFChannel(channel);
	// Set data rate
	nRF24_SetDataRate(nRF24Speed);
	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(addrLength);
	nRF24_SetDataRate(nRF24Speed);
	switch(transceiverMode){
	case nRF24_MODE_TX:
		nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
		nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR); // program address for pipe#0, must be same as TX (for Auto-ACK)
		// Set TX power (maximum)
		nRF24_SetTXPower(nRF24_TXPWR_0dBm);
		// Configure auto retransmit: 10 retransmissions with pause of 2500us in between
		nRF24_SetAutoRetr(nRF24_ARD_2500us, 15);
		// Enable Auto-ACK for pipe#0 (for ACK packets)
		nRF24_EnableAA(nRF24_PIPE0);
		// Set operational mode (PTX == transmitter)
		break;

	case nRF24_MODE_RX:
		nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
		nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, payloadLenght); // Auto-ACK: enabled, payload length: 10 bytes
		// Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
		nRF24_SetTXPower(nRF24_TXPWR_0dBm);
		break;

	}
	nRF24_SetOperationalMode(transceiverMode);
	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();
	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);
}

uint8_t Tele_EstablishConnection(nRF24_mode_t transceiverMode, nRF24_Speed_t nRF24Speed, uint8_t payloadLength, uint32_t Timeout){
	message_t nRF24_payload;
	uint32_t tickstart = HAL_GetTick();

	nRF24_payload.message.header.control = 1;
	nRF24_payload.message.header.range = Range_1;
	nRF24_payload.message.data.control.filterNum = 0;
	nRF24_payload.message.data.control.speed = nRF24Speed;
	nRF24_payload.message.data.control.txPermission = 0;

	switch(transceiverMode){

	case nRF24_MODE_TX:
		txReceived = 0;
		while(1){
			nRF24_TransmitPacketIRQ((uint8_t *) &nRF24_payload ,payloadLength);
			while(!txReceived){

			}
			if(tx_res == nRF24_TX_SUCCESS){
				nRF24_payload.message.header.range = Tele_GetRange();
				PeriphSendMessage(&nRF24_payload);
				return 1;
			}
			if((HAL_GetTick()-tickstart) > Timeout){
				return 0;
			}
		}
		break;


	case nRF24_MODE_RX:
		nRF24_ClearIRQFlags();
		rxReceived = 0;
		nRF24_CE_H();
		while(!rxReceived){
			if((HAL_GetTick()-tickstart) > Timeout){
				return 0;
			}
		}
		if(rx_res == nRF24_STATUS_RXFIFO_DATA){
			return 1;
		}
		break;

	}
	return 0;
}

uint8_t Tele_SendTransmissionChangeFrame(nRF24_Speed_t nRF24Speed, uint8_t txPermission){
	message_t nRF24_payload;

	nRF24_payload.message.header.control = 1;
	nRF24_payload.message.header.range = wirelessRange;
	nRF24_payload.message.data.control.filterNum = 0;
	nRF24_payload.message.data.control.speed = nRF24Speed;
	nRF24_payload.message.data.control.txPermission = txPermission;
	if(nRF24_SendMessage(&nRF24_payload) == 0){
		return 0;
	}
	while(nRFTxFifo.elementsInFifo != 0){}
	nRF24_CE_L();
	nRF24_SetPowerMode(nRF24_PWR_DOWN);
	return 1;
}

uint8_t nRF24_SendMessage(message_t * Message){
	Message->message.header.range = wirelessRange;
	return Fifo_PushElement((fifo_t *) &nRFTxFifo, Message);
}

uint8_t nRF24_ReceiveMessage(message_t * Message){
	if(Fifo_PullElement((fifo_t *) &nRFRxFifo, Message)){
		wirelessRange = Message->message.header.range;
		return 1;
	}
	return 0;
}

uint8_t PeriphSendMessage(message_t * Message){
	return Fifo_PushElement((fifo_t *) &periphTxFifo, Message);
}

uint8_t PeriphReceiveMessage(message_t * Message){
	return Fifo_PullElement((fifo_t *) &periphRxFifo, Message);
}


void nFR24_InterruptHandler(void){
	uint8_t status;
	message_t nRF24_payload;

	switch(transceiverMode){

	case nRF24_MODE_RX:
		if(nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY){
			rx_res = nRF24_STATUS_RXFIFO_DATA;
			pipe = nRF24_ReadPayload((uint8_t *) &nRF24_payload, (uint8_t *) &payload_length);
			Fifo_PushElement((fifo_t *) &nRFRxFifo, &nRF24_payload);
		}
		rxReceived = 1;
		break;

	case nRF24_MODE_TX:
		status = nRF24_GetStatus();
		if (status & nRF24_FLAG_MAX_RT) {
			// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
			tx_res =  nRF24_TX_MAXRT;
		}
		else if (status & nRF24_FLAG_TX_DS) {
			// Successful transmission
			tx_res = nRF24_TX_SUCCESS;

			receiverTxFifoSize = 0; // To BE CHANGED FOR GOOD VALUE

			if(Fifo_PullElement((fifo_t *) &nRFTxFifo, &nRF24_payload) == 0){
				//Send empty frame to maintain transmission
				nRF24_payload.message.header.control = 0;
				nRF24_payload.message.header.range = wirelessRange;
				nRF24_payload.message.header.canFrames = 0;
			}
			nRF24_TransmitPacketIRQ((uint8_t *) &nRF24_payload ,payload_length);
		}
		else{
			//some banana happens, flush fifo
			nRF24_FlushTX();
			tx_res = nRF24_TX_ERROR;
		}

		/* Get information about number of lost packets and number of retransmits */
		packetLostStat.nRFInto.otx = nRF24_GetRetransmitCounters();
		packetLostStat.packLostCnt += packetLostStat.nRFInto.nibbles.packLostCnt;
		nRF24_ResetPLOS();

		txReceived = 1;
		break;
	}

	nRF24_ClearIRQFlags();
}

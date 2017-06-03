/*
 * network.c
 *
 *  Created on: May 7, 2017
 *      Author: Wojciech
 */

#include "network.h"
#include "nrf24.h"
#include "fifo.h"

#define NETWORK_FIFO_SIZE	100

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

typedef union{

	struct{
		uint8_t	channel			: 7;
		uint8_t channelChopping	: 1;
	} channel;

	struct{
		uint8_t range			: 2;
		uint8_t datapresent		: 1;
		uint8_t speed			: 3;
		uint8_t reserved2		: 1;
		uint8_t channelChopping	: 1;
	} param;

} Network_settings_t;

typedef struct{
	uint8_t data[31];
} network_data_t;

typedef struct{
	network_data_t		data;
	Network_settings_t 	settings;
} Network_payload_t;

/* FIFO */
static volatile network_data_t networkTxFifoArray[NETWORK_FIFO_SIZE];
static volatile network_data_t networkRxFifoArray[NETWORK_FIFO_SIZE];
static volatile Fifo_Handle_t networkTxFifo;
static volatile Fifo_Handle_t networkRxFifo;
/* FIFO */

static volatile packetLostStat_t packetLostStat;
static volatile uint8_t payload_length;
volatile Network_settings_t networkSettings;

static volatile uint8_t txReceived;
static volatile uint8_t rxReceived;
static volatile nRF24_TXResult tx_res;
static volatile nRF24_STATUS_RXFIFO_t rx_res;
static volatile nRF24_RXResult pipe;

DeviceType_t device;

static nRF24_DataRate_t Network_DataRateConv(Network_DR_t dataRate);

Network_StatusTypeDef Network_Init(Network_t * network, uint8_t addr[], uint8_t addrLen){

	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

	if(nRF24_Init(10) != nRF24_OK){
		return Network_TIMEOUT;
	}

	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

	Network_SetName(network, addr, addrLen);

	network->channel = 0;
	network->dataRate = Network_DR_0bps;
	payload_length = 32;

	networkTxFifo = Fifo_Init( (void *) networkTxFifoArray, sizeof(network_data_t), NETWORK_FIFO_SIZE);
	networkRxFifo = Fifo_Init( (void *) networkRxFifoArray, sizeof(network_data_t), NETWORK_FIFO_SIZE);

	return Network_OK;
}

Network_StatusTypeDef Network_SetName(Network_t * network, uint8_t addr[], uint8_t addrLen){
	network->deviceAddressLen = addrLen;
	for(uint8_t i = 0; i< addrLen; i++){
		network->deviceAddress[i] = addr[i];
	}
	return Network_OK;
}

Network_StatusTypeDef Network_Find(Network_t * foundNetwork){
	//function body
	return Network_OK;
}

Network_StatusTypeDef Network_SetUpClient(Network_t * network){

	nRF24_CE_L();

	device = Client;
	network->dataRate = Network_DR_250kbps;

	nRF24_SetRFChannel(network->channel);
	// Set data rate
	nRF24_SetDataRate(Network_DataRateConv(network->dataRate));
	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(network->deviceAddressLen);
	nRF24_SetAddr(nRF24_PIPE1, network->deviceAddress); // program address for pipe
	nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, payload_length); // Auto-ACK: enabled, payload length: 10 bytes
	// Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
	nRF24_SetTXPower(nRF24_TXPWR_0dBm);
	nRF24_SetOperationalMode(nRF24_MODE_RX);
	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();
	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);

	return Network_OK;
}

Network_StatusTypeDef Network_SetUpServer(Network_t * network){

	Network_payload_t nRF24_payload;

	nRF24_CE_L();

	device = Server;
	network->dataRate = Network_DR_250kbps;

	nRF24_SetRFChannel(network->channel);
	// Set data rate
	nRF24_SetDataRate(Network_DataRateConv(network->dataRate));
	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(network->deviceAddressLen);
	nRF24_SetAddr(nRF24_PIPETX, network->deviceAddress); // program TX address
	nRF24_SetAddr(nRF24_PIPE0, network->deviceAddress); // program address for pipe#0, must be same as TX (for Auto-ACK)
	// Set TX power (maximum)
	nRF24_SetTXPower(nRF24_TXPWR_0dBm);
	// Configure auto retransmit: 10 retransmissions with pause of 2500us in between
	nRF24_SetAutoRetr(nRF24_ARD_500us, 15);
	// Enable Auto-ACK for pipe#0 (for ACK packets)
	nRF24_EnableAA(nRF24_PIPE0);
	// Set operational mode (PTX == transmitter)
	nRF24_SetOperationalMode(nRF24_MODE_TX);
	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();
	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);
	HAL_Delay(10);

	nRF24_payload.settings.param.datapresent = 0;
	nRF24_payload.settings.param.speed = Network_DataRateConv(network->dataRate);
	nRF24_payload.settings.param.range = Network_Range_0;
	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload((uint8_t *) &nRF24_payload, payload_length);
	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();

	return Network_OK;
}

Network_StatusTypeDef Network_Connect(Network_t * network, uint32_t timeout){

	uint32_t tickstart = HAL_GetTick();

	nRF24_ClearIRQFlags();
	rxReceived = 0;
	nRF24_CE_H();
	while(!rxReceived){
		if((HAL_GetTick()-tickstart) > timeout){
			return Network_TIMEOUT;
		}
	}
	if(rx_res == nRF24_STATUS_RXFIFO_DATA){
		return Network_OK;
	}
	return Network_ERROR;
}

Network_StatusTypeDef Network_Disconnect(Network_t * network);

Network_StatusTypeDef Network_Receive(uint8_t * data, uint8_t * dataLen){
	if(Fifo_OK == Fifo_PullElement( (Fifo_Handle_t *) &networkRxFifo, data)){
		*dataLen = networkRxFifo.uxItemSize;
		return Network_OK;
	}
	*dataLen = 0;
	return Network_ERROR;
}

Network_StatusTypeDef Network_Send(uint8_t * data, uint8_t dataLen){
	if(dataLen == networkTxFifo.uxItemSize){
		if(Fifo_OK == Fifo_PushElement( (Fifo_Handle_t *) &networkTxFifo, data)){
			return Network_OK;
		}
		return Network_ERROR;
	}
	return Network_ERROR;
}

Network_DR_t Network_GetDataRate(Network_t * network){
	return network->dataRate;
}

uint8_t Network_GetChannel(Network_t * network){
	return network->channel;
}

Network_Range_t Network_GetRange(void){

	static Network_Range_t range;

	switch(device){

	case Server:
		if(packetLostStat.nRFInto.nibbles.autoRetrCnt < 6){
			return Network_Range_3;
		}
		else if(packetLostStat.nRFInto.nibbles.autoRetrCnt < 11){
			return Network_Range_2;
		}
		else{
			return Network_Range_1;
		}
		break;

	case Client:
		if(networkSettings.param.channelChopping == 0){
			range = networkSettings.param.range;
		}
		return range;
		break;

	default:
		return range;
		break;
	}
}

static nRF24_DataRate_t Network_DataRateConv(Network_DR_t dataRate){
	switch(dataRate){

	case Network_DR_250kbps:
		return nRF24_DR_250kbps;

	case Network_DR_1Mbps:
		return nRF24_DR_1Mbps;

	case Network_DR_2Mbps:
		return nRF24_DR_2Mbps;

	default:
		return nRF24_DR_250kbps;
		break;
	}
}

void Network_InterruptHandler(void){

	uint8_t status;
	Network_payload_t nRF24_payload;

	switch(device){

	case Client:
		if(nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY){
			rx_res = nRF24_STATUS_RXFIFO_DATA;
			pipe = nRF24_ReadPayload((uint8_t *) &nRF24_payload, (uint8_t *) &payload_length);
			if(nRF24_payload.settings.param.datapresent){
				if(Fifo_PushElement( (Fifo_Handle_t *) &networkRxFifo, &(nRF24_payload.data) ) == Fifo_FULL){
				}
			}
			networkSettings = nRF24_payload.settings;
		}
		rxReceived = 1;
		break;

	case Server:
		status = nRF24_GetStatus();
		if (status & nRF24_FLAG_MAX_RT) {
			// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
			tx_res =  nRF24_TX_MAXRT;
		}
		else if (status & nRF24_FLAG_TX_DS) {
			// Successful transmission
			tx_res = nRF24_TX_SUCCESS;

			nRF24_payload.settings = networkSettings;
			nRF24_payload.settings.param.range = Network_GetRange();
			if(Fifo_PullElement( (Fifo_Handle_t *) &networkTxFifo, &(nRF24_payload.data) ) == Fifo_EMPTY){
				nRF24_payload.settings.param.datapresent = 0;
			}
			else{
				nRF24_payload.settings.param.datapresent = 1;
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

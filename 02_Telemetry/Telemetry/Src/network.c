/*
 * network.c
 *
 *  Created on: May 7, 2017
 *      Author: Wojciech
 */

#include "network.h"
#include "nrf24.h"
#include "fifo.h"
#include "tim.h"

#define NETWORK_FIFO_SIZE	100

/* FIFO */
typedef struct NetworkFIFO{
	network_data_t networkTxFifoArray[NETWORK_FIFO_SIZE];
	network_data_t networkRxFifoArray[NETWORK_FIFO_SIZE];
	Fifo_Handle_t networkTxFifo;
	Fifo_Handle_t networkRxFifo;
} NetworkFIFO_t;

static volatile NetworkFIFO_t NetworkFIFO[NETWORK_NUMBER];
/* FIFO */

Network_t network[MAX_NETWORK_NUMBER];

static nRF24_DataRate_t Network_DataRateConv(Network_DR_t dataRate);

void Network_DeviceInit(void){
	for(uint8_t networkCtr = 0; networkCtr < MAX_NETWORK_NUMBER; networkCtr++){
		network[networkCtr].ID = networkCtr;
	}
}

Network_StatusTypeDef Network_Init(Network_t * network, uint8_t addr[], uint8_t addrLen){

	network->nRF24device = &nRF24Devices[network->ID];

	nRF24_DIS_IRQ(network->nRF24device);

	if(nRF24_Init(network->nRF24device, 10) != nRF24_OK){
		return Network_TIMEOUT;
	}

	nRF24_ENA_IRQ(network->nRF24device);

	Network_SetName(network, addr, addrLen);

	network->channel = 0;
	network->dataRate = Network_DR_0bps;
	network->range = Network_Range_0;
	network->payload_length = 32;

	NetworkFIFO[network->ID].networkTxFifo = Fifo_Init( (void *) NetworkFIFO[network->ID].networkTxFifoArray, sizeof(network_data_t), NETWORK_FIFO_SIZE);
	NetworkFIFO[network->ID].networkRxFifo = Fifo_Init( (void *) NetworkFIFO[network->ID].networkRxFifoArray, sizeof(network_data_t), NETWORK_FIFO_SIZE);

	return Network_OK;
}

void Network_SetName(Network_t * network, uint8_t addr[], uint8_t addrLen){
	network->deviceAddressLen = addrLen;
	for(uint8_t i = 0; i< addrLen; i++){
		network->deviceAddress[i] = addr[i];
	}
}

Network_StatusTypeDef Network_Find(Network_t * foundNetwork){
	//function body
	return Network_OK;
}

Network_StatusTypeDef Network_SetUpClient(Network_t * network){

	nRF24_CE_L(network->nRF24device);
	nRF24_SetPowerMode(network->nRF24device, nRF24_PWR_DOWN);

	network->deviceType = Client;
	network->setDataRate = Network_DR_250kbps;

	nRF24_SetRFChannel(network->nRF24device, network->channel);
	// Set data rate
	nRF24_SetDataRate(network->nRF24device, Network_DataRateConv(network->setDataRate));
	// Set CRC scheme
	nRF24_SetCRCScheme(network->nRF24device, nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(network->nRF24device, network->deviceAddressLen);
	nRF24_SetAddr(network->nRF24device, nRF24_PIPE0, network->deviceAddress); // program address for pipe
	nRF24_SetRXPipe(network->nRF24device, nRF24_PIPE0, nRF24_AA_ON, network->payload_length);// Auto-ACK: enabled, payload length
	nRF24_EnableACKPayload(network->nRF24device, nRF24_PIPE0);
	// Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
	nRF24_SetTXPower(network->nRF24device, nRF24_TXPWR_0dBm);
	nRF24_SetOperationalMode(network->nRF24device, nRF24_MODE_RX);
	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags(network->nRF24device);
	// Wake the transceiver
	nRF24_SetPowerMode(network->nRF24device, nRF24_PWR_UP);

	return Network_OK;
}

void Network_SetDataRate(Network_t * network, Network_DR_t dataRate){
	if(dataRate != network->setDataRate){
		Network_Disconnect(network);
		network->setDataRate = dataRate;
		nRF24_SetDataRate(network->nRF24device, Network_DataRateConv(network->setDataRate));
		nRF24_CE_H(network->nRF24device);
	}
}

void Network_IncreaseDataRate(Network_t * network){
	switch(network->setDataRate){

	case Network_DR_250kbps:
		Network_SetDataRate(network, Network_DR_1Mbps);
		break;

	case Network_DR_1Mbps:
		Network_SetDataRate(network, Network_DR_2Mbps);
		break;

	default:
		break;

	}
}

void Network_DecreaseDataRate(Network_t * network){
	switch(network->setDataRate){

	case Network_DR_1Mbps:
		Network_SetDataRate(network, Network_DR_250kbps);
		break;

	case Network_DR_2Mbps:
		Network_SetDataRate(network, Network_DR_1Mbps);
		break;

	default:
		break;

	}
}

void Network_ReqIncreaseDataRate(Network_t * network){
	switch(network->dataRate){

	case Network_DR_250kbps:
		network->settings.param.speed = Network_DR_1Mbps;
		break;

	case Network_DR_1Mbps:
		network->settings.param.speed = Network_DR_2Mbps;
		break;

	default:
		break;

	}
}

void Network_ReqDecreaseDataRate(Network_t * network){
	switch(network->dataRate){

	case Network_DR_1Mbps:
		network->settings.param.speed = Network_DR_250kbps;
		break;

	case Network_DR_2Mbps:
		network->settings.param.speed = Network_DR_1Mbps;
		break;

	default:
		break;

	}
}

Network_StatusTypeDef Network_SetUpServer(Network_t * network){

	Network_frame_t nRF24_payload;

	nRF24_CE_L(network->nRF24device);
	nRF24_SetPowerMode(network->nRF24device, nRF24_PWR_DOWN);

	network->deviceType = Server;
	network->setDataRate = Network_DR_250kbps;
	network->settings.param.datapresent = 0;
	network->settings.param.channelChopping = 0;
	network->settings.param.speed = Network_DR_250kbps;
	network->settings.param.range = Network_Range_0;

	nRF24_SetRFChannel(network->nRF24device, network->channel);
	// Set data rate
	nRF24_SetDataRate(network->nRF24device, Network_DataRateConv(network->setDataRate));
	// Set CRC scheme
	nRF24_SetCRCScheme(network->nRF24device, nRF24_CRC_2byte);
	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(network->nRF24device, network->deviceAddressLen);
	nRF24_SetAddr(network->nRF24device, nRF24_PIPETX, network->deviceAddress); // program TX address
	nRF24_SetAddr(network->nRF24device, nRF24_PIPE0, network->deviceAddress); // program address for pipe#0, must be same as TX (for Auto-ACK)
	// Set TX power (maximum)
	nRF24_SetTXPower(network->nRF24device, nRF24_TXPWR_0dBm);
	// Configure auto retransmit: 10 retransmissions with pause of 2500us in between
	nRF24_SetAutoRetr(network->nRF24device, nRF24_ARD_500us, 15);
	// Enable Auto-ACK for pipe#0 (for ACK packets)
	nRF24_EnableAA(network->nRF24device, nRF24_PIPE0);
	nRF24_EnableACKPayload(network->nRF24device, nRF24_PIPE0);
	// Set operational mode (PTX == transmitter)
	nRF24_SetOperationalMode(network->nRF24device, nRF24_MODE_TX);
	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags(network->nRF24device);
	// Wake the transceiver
	nRF24_SetPowerMode(network->nRF24device, nRF24_PWR_UP);
	HAL_Delay(5);

	nRF24_payload.settings = network->settings;
	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(network->nRF24device, (uint8_t *) &nRF24_payload, network->payload_length);
	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H(network->nRF24device);

	return Network_OK;
}

Network_StatusTypeDef Network_Connect(Network_t * network, uint32_t timeout){

	uint32_t tickstart = HAL_GetTick();

	nRF24_ClearIRQFlags(network->nRF24device);
	network->rxReceived = 0;
	nRF24_CE_H(network->nRF24device);
	while(!network->rxReceived){
		if((HAL_GetTick()-tickstart) > timeout){
			return Network_TIMEOUT;
		}
	}
	if(network->rx_res == nRF24_STATUS_RXFIFO_DATA){
		return Network_OK;
	}
	return Network_ERROR;
}

void Network_Disconnect(Network_t * network){
	nRF24_CE_L(network->nRF24device);
	nRF24_ClearIRQFlags(network->nRF24device);
}

Network_StatusTypeDef Network_IsConnectedToServer(void);

Network_StatusTypeDef Network_IsClientConnected(void);

Network_StatusTypeDef Network_Receive(Network_t * network, uint8_t * data, uint8_t * dataLen){
	if(FIFO_OK == Fifo_PullElement( (Fifo_Handle_t *) &(NetworkFIFO[network->ID].networkRxFifo), data)){
		*dataLen = NetworkFIFO[network->ID].networkRxFifo.uxItemSize;
		return Network_OK;
	}
	*dataLen = 0;
	return Network_ERROR;
}

Network_StatusTypeDef Network_Send(Network_t * network, uint8_t * data, uint8_t dataLen){
	if(dataLen == NetworkFIFO[network->ID].networkTxFifo.uxItemSize){
		if(FIFO_OK == Fifo_PushElement( (Fifo_Handle_t *) &(NetworkFIFO[network->ID].networkTxFifo), data)){
			return Network_OK;
		}
		return Network_ERROR;
	}
	return Network_ERROR;
}

Network_DR_t Network_GetDataRate(Network_t * network){
	if(network->range == Network_Range_0){
		return Network_DR_0bps;
	}
	return network->dataRate;
}

uint8_t Network_GetChannel(Network_t * network){
	return network->channel;
}

Network_Range_t Network_GetRange(Network_t * network){

	switch(network->deviceType){

	case Server:
		if(network->packetLostStat.nRFInto.nibbles.autoRetrCnt < 2){
			return Network_Range_3;
		}
		else if(network->packetLostStat.nRFInto.nibbles.autoRetrCnt < 5){
			return Network_Range_2;
		}
		else{
			return Network_Range_1;
		}
		break;

	case Client:
		return network->range;
		break;

	}

	return Network_Range_0;
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

void Network_InterruptHandler(Network_t * network){

	uint8_t status;
	Network_frame_t nRF24_payload;

	switch(network->deviceType){
	//testowanie duplexu
	case Client:
		if(nRF24_GetStatus_RXFIFO(network->nRF24device) != nRF24_STATUS_RXFIFO_EMPTY){
			network->rx_res = nRF24_STATUS_RXFIFO_DATA;
			network->pipe = nRF24_ReadPayload(network->nRF24device, (uint8_t *) &nRF24_payload, (uint8_t *) &(network->payload_length));
			if((nRF24_payload.settings.param.channelChopping == 0) && (nRF24_payload.settings.param.datapresent == 1)){
				if(Fifo_PushElement( (Fifo_Handle_t *) &(NetworkFIFO[network->ID].networkRxFifo), &(nRF24_payload.data) ) == FIFO_FULL){
				}
			}
			network->settings = nRF24_payload.settings;

			if(Fifo_PullElement( (Fifo_Handle_t *) &(NetworkFIFO[network->ID].networkTxFifo), &(nRF24_payload.data) ) == FIFO_EMPTY){
				nRF24_payload.settings.param.datapresent = 0;
			}
			else{
				nRF24_payload.settings.param.datapresent = 1;
			}
			nRF24_WriteACKPayload(network->nRF24device, (uint8_t *) &nRF24_payload ,network->payload_length, nRF24_PIPE0);
		}
		network->rxReceived = 1;
		break;

	case Server:
		status = nRF24_GetStatus(network->nRF24device);
		if (status & nRF24_FLAG_MAX_RT) {
			// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
			network->tx_res =  nRF24_TX_MAXRT;
		}
		else if (status & nRF24_FLAG_TX_DS) {
			// Successful transmission
			network->tx_res = nRF24_TX_SUCCESS;

			nRF24_payload.settings = network->settings;
			if(Fifo_PullElement( (Fifo_Handle_t *) &(NetworkFIFO[network->ID].networkTxFifo), &(nRF24_payload.data) ) == FIFO_EMPTY){
				nRF24_payload.settings.param.datapresent = 0;
			}
			else{
				nRF24_payload.settings.param.datapresent = 1;
			}
			nRF24_TransmitPacketIRQ(network->nRF24device, (uint8_t *) &nRF24_payload ,32);//network->payload_length);
		}
		else{
			//some banana happens, flush fifo
			nRF24_FlushTX(network->nRF24device);
			network->tx_res = nRF24_TX_ERROR;
		}

		/* Get information about number of lost packets and number of retransmits */
		network->packetLostStat.nRFInto.otx = nRF24_GetRetransmitCounters(network->nRF24device);
		network->packetLostStat.packLostCnt += network->packetLostStat.nRFInto.nibbles.packLostCnt;
		nRF24_ResetPLOS(network->nRF24device);

		network->txReceived = 1;
		break;
	}

	nRF24_ClearIRQFlags(network->nRF24device);
}

void Network_TimerInterruptHandler(Network_t * network){

	static uint8_t IncReq, DecReq;

	switch(network->deviceType){

	case Client:
		if(network->rxReceived == 0){
			network->range = Network_Range_0;
			Network_SetDataRate(network, Network_DR_250kbps);
			network->dataRate = Network_DR_0bps;
		}
		else{
			network->rxReceived = 0;
			if(network->settings.param.channelChopping == 0){
				network->range = network->settings.param.range;
				Network_SetDataRate(network,network->settings.param.speed);
			}
			network->dataRate = network->setDataRate;
		}
		break;

	case Server:

		if(network->txReceived == 0){
			network->range = Network_Range_0;
			network->settings.param.speed = Network_DR_250kbps;
			Network_SetDataRate(network, Network_DR_250kbps);
			network->dataRate = Network_DR_0bps;
		}
		else if(network->tx_res != nRF24_TX_SUCCESS){
			network->txReceived = 0;
			network->range = Network_Range_0;
			network->settings.param.speed = Network_DR_250kbps;
			Network_SetDataRate(network, Network_DR_250kbps);
			network->dataRate = Network_DR_0bps;
		}
		else{
			network->txReceived = 0;
			network->settings.param.channelChopping = 0;
			network->settings.param.range = Network_GetRange(network) ;
			network->dataRate = network->setDataRate;


			if(IncReq){
				Network_IncreaseDataRate(network);
				IncReq = 0;
			}
			else if(DecReq){
				Network_DecreaseDataRate(network);
				DecReq = 0;
			}
			else if(network->settings.param.range > Network_Range_1){
				Network_ReqIncreaseDataRate(network);
				IncReq = 1;
			}
			else{
				Network_ReqDecreaseDataRate(network);
				DecReq = 1;
			}


		}
		break;
	}

}

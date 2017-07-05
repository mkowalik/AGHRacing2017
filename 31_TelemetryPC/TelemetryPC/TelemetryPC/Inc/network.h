/*
 * network.h
 *
 *  Created on: May 7, 2017
 *      Author: Wojciech
 */

#ifndef NETWORK_H_
#define NETWORK_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include "nrf24.h"

#define MAX_ADDR_LEN	5
#define MAX_NETWORK_NUMBER	NRF24_DEVICES_NUMBER
#define NETWORK_NUMBER	1

#if(NETWORK_NUMBER > MAX_NETWORK_NUMBER)
error
#endif

typedef enum{
  Network_OK       = 0x00,
  Network_ERROR    = 0x01,
  Network_BUSY     = 0x02,
  Network_TIMEOUT  = 0x03
} Network_StatusTypeDef;

typedef enum{
	Server,
	Client
} DeviceType_t;

typedef enum{
	Network_DR_0bps		= 0x00,
	Network_DR_250kbps 	= 0x01,
	Network_DR_1Mbps	= 0x02,
	Network_DR_2Mbps	= 0x03
} Network_DR_t;

typedef enum{
	Network_Range_0		= 0x00,
	Network_Range_1		= 0x01,
	Network_Range_2		= 0x02,
	Network_Range_3		= 0x03
} Network_Range_t;

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
	Network_DR_t dataRate;
	Network_DR_t setDataRate;
	Network_Range_t range;
	uint8_t channel;
	uint8_t deviceAddress[MAX_ADDR_LEN];
	uint8_t deviceAddressLen;
	nRF24_Device_t * nRF24device;
	DeviceType_t deviceType;
	uint8_t ID;
	volatile packetLostStat_t packetLostStat;
	volatile Network_settings_t settings;
	volatile uint8_t payload_length;
	volatile uint8_t txReceived;
	volatile uint8_t rxReceived;
	volatile nRF24_TXResult tx_res;
	volatile nRF24_RXResult pipe;
	volatile nRF24_STATUS_RXFIFO_t rx_res;
} Network_t;

typedef struct{
	uint8_t data[31];
} network_data_t;

typedef struct{
	network_data_t		data;
	Network_settings_t 	settings;
} Network_frame_t;

extern Network_t network[MAX_NETWORK_NUMBER];

void Network_DeviceInit(void);
Network_StatusTypeDef Network_Init(Network_t * network, uint8_t addr[], uint8_t addrLen);
void Network_SetName(Network_t * network, uint8_t addr[], uint8_t addrLen);
Network_StatusTypeDef Network_Find(Network_t * foundNetwork);
Network_StatusTypeDef Network_SetUpServer(Network_t * network);
Network_StatusTypeDef Network_SetUpClient(Network_t * network);
Network_StatusTypeDef Network_Connect(Network_t * network, uint32_t timeout);
void Network_Disconnect(Network_t * network);
Network_StatusTypeDef Network_IsConnectedToServer(void);
Network_StatusTypeDef Network_IsClientConnected(void);
Network_StatusTypeDef Network_Receive(Network_t * network, uint8_t * data, uint8_t * dataLen);
Network_StatusTypeDef Network_Send(Network_t * network, uint8_t * data, uint8_t dataLen);
Network_DR_t Network_GetDataRate(Network_t * network);
uint8_t Network_GetChannel(Network_t * network);
Network_Range_t Network_GetRange(Network_t * network);

void Network_InterruptHandler(Network_t * network);
void Network_TimerInterruptHandler(Network_t * network);

#ifdef __cplusplus
}
#endif
#endif /* NETWORK_H_ */

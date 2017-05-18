/*
 * network.h
 *
 *  Created on: May 7, 2017
 *      Author: Wojciech
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "stm32f0xx_hal.h"

#define MAX_ADDR_LEN	5

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
	Network_DR_t dataRate;
	uint8_t channel;
	uint8_t deviceAddress[MAX_ADDR_LEN];
	uint8_t deviceAddressLen;
} Network_t;

Network_StatusTypeDef Network_Init(Network_t * network, uint8_t addr[], uint8_t addrLen);
Network_StatusTypeDef Network_SetName(Network_t * network, uint8_t addr[], uint8_t addrLen);
Network_StatusTypeDef Network_Find(Network_t * foundNetwork);
Network_StatusTypeDef Network_SetUpServer(Network_t * network);
Network_StatusTypeDef Network_SetUpClient(Network_t * network);
Network_StatusTypeDef Network_Connect(Network_t * network, uint32_t timeout);
Network_StatusTypeDef Network_Disconnect(Network_t * network);
Network_StatusTypeDef Network_Receive(uint8_t * data, uint8_t * dataLen);
Network_StatusTypeDef Network_Send(uint8_t * data, uint8_t dataLen);
Network_DR_t Network_GetDataRate(Network_t * network);
uint8_t Network_GetChannel(Network_t * network);
Network_Range_t Network_GetRange(void);

void Network_InterruptHandler(void);

#endif /* NETWORK_H_ */

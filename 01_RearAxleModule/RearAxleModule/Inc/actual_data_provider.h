/*
 * actual_data_provider.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef ACTUAL_DATA_PROVIDER_H_
#define ACTUAL_DATA_PROVIDER_H_

#include "stm32f0xx_hal.h"

// true_value = (can_value * multiplier) / divider + offset

typedef enum {
	STOP_DATA_CHANNEL	= 0
} Channel_TypeDef;

#define DATA_CHANNELS_NUMBER	1

void ActualDataProvider_init();
uint16_t ActualDataProvider_getValue(Channel_TypeDef channel);
uint16_t ActualDataProvider_getOffset(Channel_TypeDef channel);
void ActualDataProvider_thread();

#endif /* ACTUAL_DATA_PROVIDER_H_ */

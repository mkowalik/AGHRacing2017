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
	RPM_DATA_CHANNEL 			= 0,
	CLT_DATA_CHANNEL 			= 1,
	FUEL_LEVEL_DATA_CHANNEL 	= 2,
	OIL_PRESSURE_DATA_CHANNEL 	= 3,
	BATT_VOLTAGE_DATA_CHANNEL 	= 4,
	GEAR_DATA_CHANNEL 			= 5
} Channel_TypeDef;

#define DATA_CHANNELS_NUMBER	6

void ActualDataProvider_init();
RTC_TimeTypeDef ActualDataProvider_getDataArrivalTime(Channel_TypeDef channel);
uint16_t ActualDataProvider_getValue(Channel_TypeDef channel);
uint16_t ActualDataProvider_getMultiplier(Channel_TypeDef channel);
uint16_t ActualDataProvider_getDivider(Channel_TypeDef channel);
uint16_t ActualDataProvider_getOffset(Channel_TypeDef channel);
void ActualDataProvider_thread();

#endif /* ACTUAL_DATA_PROVIDER_H_ */

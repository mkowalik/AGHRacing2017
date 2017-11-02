/*
 * actual_data_provider.h
 *
 *  Created on: 03.06.2017
 *      Author: Kowalik
 */

#ifndef ACTUAL_DATA_PROVIDER_H_
#define ACTUAL_DATA_PROVIDER_H_

#include "stm32f1xx_hal.h"
#include "can_data.h"

// true_value = (can_value * multiplier) / divider + offset

void ActualDataProvider_init();
void ActualDataProvider_thread();
CANData_TypeDef ActualDataProvider_getData(uint32_t channelId); //channel value must be between 0 and 2^11=2048 (CAN_STD_ID range)

#endif /* ACTUAL_DATA_PROVIDER_H_ */

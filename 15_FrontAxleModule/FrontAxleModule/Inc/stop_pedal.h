/*
 * stop_pedal.h
 *
 *  Created on: May 11, 2017
 *      Author: Wojciech
 */

#ifndef STOP_PEDAL_H_
#define STOP_PEDAL_H_

#include <stdbool.h>

bool StopPedal_Read(void);
void StopPedal_can_data_extract(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size, void * value);
float StopPedal_can_data_calc(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size);

#endif /* STOP_PEDAL_H_ */

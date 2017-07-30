/*
 * stop_light.h
 *
 *  Created on: May 11, 2017
 *      Author: Wojciech
 */

#include <inttypes.h>

#ifndef STOP_LIGHT_H_
#define STOP_LIGHT_H_

void StopLight_Set(void);
void StopLight_Clr(void);
float StopLight_can_data_calc(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size);

#endif /* STOP_LIGHT_H_ */

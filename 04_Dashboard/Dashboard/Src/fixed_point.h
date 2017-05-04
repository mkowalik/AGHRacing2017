/*
 * fixed_point.h
 *
 *  Created on: 05.05.2017
 *      Author: kowalik
 */

#ifndef FIXED_POINT_H_
#define FIXED_POINT_H_

#include "stm32f0xx_hal.h"

typedef struct {
	int32_t	integer;
	uint8_t	decimalDigits;
} FixedPoint;

uint8_t a_greater_b(FixedPoint a, FixedPoint b);
uint8_t a_less_b(FixedPoint a, FixedPoint b);

#endif /* FIXED_POINT_H_ */


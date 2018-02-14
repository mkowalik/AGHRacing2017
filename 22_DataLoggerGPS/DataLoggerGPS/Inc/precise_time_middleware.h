/*
 * precise_time_middleware.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef PRECISE_TIME_MIDDLEWARE_H_
#define PRECISE_TIME_MIDDLEWARE_H_

#include "stdint.h"
#include "rtc.h"

typedef struct {
	RTC_DateTypeDef date;
	uint32_t unixTime;
	uint16_t miliseconds;
} PreciseTime_TypeDef;

void PreciseTimeMiddleware_init();
uint32_t PreciseTimeMiddleware_getUnixTime();
PreciseTime_TypeDef PreciseTimeMiddleware_getPreciseTime();

#endif /* PRECISE_TIME_MIDDLEWARE_H_ */

/*
 * precise_time_middleware.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef RTC_MIDDLEWARE_H_
#define RTC_MIDDLEWARE_H_

#include "stdint.h"
#include "rtc.h"

typedef struct {
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
} DateTime_TypeDef;

typedef enum {
	RTCMiddleware_Status_OK = 0,
	RTCMiddleware_Status_NotInitialisedError,
	RTCMiddleware_Status_Error
} RTCMiddleware_Status_TypeDef;

typedef enum {
	RTCMiddleware_State_UnInitialized = 0,
	RTCMiddleware_State_Ready
} RTCMiddleware_State_TypeDef;

typedef struct {
	RTC_HandleTypeDef*			pRTCHandler;
	RTCMiddleware_State_TypeDef	state;
} RTCMiddleware_TypeDef;

RTCMiddleware_Status_TypeDef RTCMiddleware_init(RTCMiddleware_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler);
RTCMiddleware_Status_TypeDef RTCMiddleware_getDateAndTime(RTCMiddleware_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime);


#endif /* RTC_MIDDLEWARE_H_ */

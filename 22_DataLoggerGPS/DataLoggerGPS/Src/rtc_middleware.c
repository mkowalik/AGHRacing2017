/*
 * precise_time_middleware.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#include "rtc_middleware.h"


RTCMiddleware_Status_TypeDef RTCMiddleware_init(RTCMiddleware_TypeDef* pSelf, RTC_HandleTypeDef* pRTCHandler){

	if (pSelf->state != RTCMiddleware_State_UnInitialized){
		return RTCMiddleware_Status_Error;
	}

	if (pRTCHandler == NULL){
		return RTCMiddleware_Status_Error;
	}

	pSelf->pRTCHandler = pRTCHandler;
	pSelf->state = RTCMiddleware_State_Ready;

	return RTCMiddleware_Status_OK;
}

RTCMiddleware_Status_TypeDef RTCMiddleware_getDateAndTime(RTCMiddleware_TypeDef* pSelf, DateTime_TypeDef* pRetDateTime){

	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	HAL_StatusTypeDef status;

	status = HAL_RTC_GetDate(pSelf->pRTCHandler, &date, RTC_FORMAT_BIN);
	if (status != HAL_OK){
		return RTCMiddleware_Status_Error;
	}

	status = HAL_RTC_GetTime(pSelf->pRTCHandler, &time, RTC_FORMAT_BIN);
	if (status != HAL_OK){
		return RTCMiddleware_Status_Error;
	}

	pRetDateTime->year = 	date.Year;
	pRetDateTime->month = 	date.Month;
	pRetDateTime->day = 	date.Date;
	pRetDateTime->hour = 	time.Hours;
	pRetDateTime->minute = 	time.Minutes;
	pRetDateTime->second = 	time.Seconds;

	return RTCMiddleware_Status_OK;

}



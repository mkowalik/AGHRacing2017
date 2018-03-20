/*
 * ms_timer_middleware.h
 *
 *  Created on: 19.03.2018
 *      Author: Kowalik
 */

#ifndef MS_TIMER_MIDDLEWARE_H_
#define MS_TIMER_MIDDLEWARE_H_

#include "stm32f1xx_hal.h"

typedef enum {
	MSTimerMiddleware_State_NotInitialised = 0,
	MSTimerMiddleware_State_Idle,
	MSTimerMiddleware_State_Suspended,
	MSTimerMiddleware_State_Running
} MSTimerMiddleware_State_TypeDef;

typedef enum {
	MSTimerMiddleware_Status_OK = 0,
	MSTimerMiddleware_Status_Error_NotInitialised,
	MSTimerMiddleware_Status_Error
} MSTimerMiddleware_Status_TypeDef;

typedef struct  {
	MSTimerMiddleware_State_TypeDef	state;
	uint32_t						uiStartTickValue;
	uint32_t						uiStopTickValue;
} MSTimerMiddleware_TypeDef;

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_init(MSTimerMiddleware_TypeDef* pSelf);
MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_startCounting(MSTimerMiddleware_TypeDef* pSelf);
MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_stopCounting(MSTimerMiddleware_TypeDef* pSelf);
MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_resetCounter(MSTimerMiddleware_TypeDef* pSelf);
MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_getMSTime(MSTimerMiddleware_TypeDef* pSelf, uint32_t* pRetTime);

#endif /* MS_TIMER_MIDDLEWARE_H_ */

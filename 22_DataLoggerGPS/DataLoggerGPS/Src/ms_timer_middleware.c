/*
 * ms_timer_middleware.c
 *
 *  Created on: 19.03.2018
 *      Author: Kowalik
 */

#include "ms_timer_middleware.h"

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_init(MSTimerMiddleware_TypeDef* pSelf){

	if (pSelf->state != MSTimerMiddleware_State_NotInitialised){
		return MSTimerMiddleware_Status_Error;
	}

	pSelf->state = MSTimerMiddleware_State_Idle;

	return MSTimerMiddleware_Status_OK;
}

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_startCounting(MSTimerMiddleware_TypeDef* pSelf){

	if (pSelf->state == MSTimerMiddleware_State_NotInitialised){
		return MSTimerMiddleware_State_NotInitialised;
	}

	if (pSelf->state == MSTimerMiddleware_State_Running){
		return MSTimerMiddleware_Status_Error;
	}

	switch (pSelf->state){
	case MSTimerMiddleware_State_Suspended:
		pSelf->uiStartTickValue = pSelf->uiStartTickValue - (HAL_GetTick() - pSelf->uiStopTickValue);
		break;
	case MSTimerMiddleware_State_Idle:
	default:
		pSelf->uiStartTickValue = HAL_GetTick();
		break;
	}

	pSelf->state = MSTimerMiddleware_State_Running;

	return MSTimerMiddleware_Status_OK;

}

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_stopCounting(MSTimerMiddleware_TypeDef* pSelf){

	if (pSelf->state == MSTimerMiddleware_State_NotInitialised){
		return MSTimerMiddleware_State_NotInitialised;
	}

	if ((pSelf->state == MSTimerMiddleware_State_Idle) || ((pSelf->state == MSTimerMiddleware_State_Suspended))){
		return MSTimerMiddleware_Status_Error;
	}

	pSelf->uiStopTickValue = HAL_GetTick();
	pSelf->state = MSTimerMiddleware_State_Suspended;

	return MSTimerMiddleware_Status_OK;
}

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_resetCounter(MSTimerMiddleware_TypeDef* pSelf){

	if (pSelf->state == MSTimerMiddleware_State_NotInitialised){
		return MSTimerMiddleware_State_NotInitialised;
	}

	if (pSelf->state == MSTimerMiddleware_State_Running){
		return MSTimerMiddleware_Status_Error;
	}

	pSelf->state = MSTimerMiddleware_State_Idle;

	return MSTimerMiddleware_Status_OK;

}

MSTimerMiddleware_Status_TypeDef MSTimerMiddleware_getMSTime(MSTimerMiddleware_TypeDef* pSelf, uint32_t* pRetTime){

	if (pSelf->state == MSTimerMiddleware_State_NotInitialised){
		return MSTimerMiddleware_State_NotInitialised;
	}

	switch (pSelf->state){
	case MSTimerMiddleware_State_Idle:
		*pRetTime = 0;
		break;
	case MSTimerMiddleware_State_Suspended:
		*pRetTime = pSelf->uiStopTickValue - pSelf->uiStartTickValue;
		break;
	case MSTimerMiddleware_State_Running:
	default:
		*pRetTime = HAL_GetTick() - pSelf->uiStartTickValue;
		break;
	}

	return MSTimerMiddleware_Status_OK;

}

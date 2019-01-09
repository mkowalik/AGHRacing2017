/*
 * action_scheduler.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include "config.h"
#include "data_saver_buffer.h"
#include "can_receiver.h"
#include "rtc_middleware.h"
#include "ms_timer_middleware.h"

typedef enum {
	ActionScheduler_Status_OK = 0,
	ActionScheduler_Status_UnInitializedError,
	ActionScheduler_Status_Error
} ActionScheduler_Status_TypeDef;


/**
 * UnInitialized --> SystemConfig --> Idle --> LogInit --> Logging --> LogClose
 * 									  /\									|
 * 						 			  |										|
 * 						 			  |_____________________________________|
 */
typedef enum {
	ActionScheduler_State_UnInitialized = 0,
	ActionScheduler_State_Idle,
	ActionScheduler_State_LogInit,
	ActionScheduler_State_Logging,
	ActionScheduler_State_LogClose
} ActionScheduler_State_TypeDef;

typedef struct {
	ActionScheduler_State_TypeDef	state;
	ConfigDataManager_TypeDef*		pConfigManager;
	DataSaverBuffer_TypeDef*		pDataSaver;
	CANReceiver_TypeDef*			pCANReceiver;
	RTCMiddleware_TypeDef*			pRTCMiddleware;
	uint32_t						logStartTime;
} ActionScheduler_TypeDef;


ActionScheduler_Status_TypeDef ActionScheduler_init(ActionScheduler_TypeDef* pSelf, ConfigDataManager_TypeDef* pConfigManager,
		DataSaverBuffer_TypeDef* pDataSaver, CANReceiver_TypeDef* pCANReceiver, RTCMiddleware_TypeDef* pRTCMiddleware);
ActionScheduler_Status_TypeDef ActionScheduler_startScheduler(ActionScheduler_TypeDef* pSelf);

#endif /* ACTION_SCHEDULER_H_ */

/*
 * action_manager.h
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */

#ifndef ACTION_MANAGER_H_
#define ACTION_MANAGER_H_

#include "config.h"
#include "can_data.h"

typedef enum {
	ActionManager_Status_OK = 0,
	ActionManager_Status_IDLE,
	ActionManager_Status_SYSTEM_INIT,
	ActionManager_Status_LOG_INIT,
	ActionManager_Status_LOGGING,
	ActionManager_Status_ERROR
} ActionManager_Status_TypeDef;

typedef struct {
	ActionManager_Status_TypeDef	state;
} ActionManager_TypeDef;

typedef void (*ActionFunctionPointer_TypeDef)(CANData_TypeDef);

ActionManager_Status_TypeDef ActionManager_init(ConfigChannel_TypeDef* pChannel, void (*pAction)());
ActionManager_Status_TypeDef ActionManager_registerAction(ConfigChannel_TypeDef* pChannel, ActionFunctionPointer_TypeDef);
ActionManager_Status_TypeDef ActionManager_registerDefaultAction(ActionFunctionPointer_TypeDef);
ActionManager_Status_TypeDef ActionManager_thread();


#endif /* ACTION_MANAGER_H_ */

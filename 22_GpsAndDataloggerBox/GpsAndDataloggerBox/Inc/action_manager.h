/*
 * action_manager.h
 *
 *  Created on: 02.08.2017
 *      Author: Kowalik
 */

#ifndef ACTION_MANAGER_H_
#define ACTION_MANAGER_H_

#include "can_data.h"

typedef enum {
	ActionManager_State_IDLE = 0,
	ActionManager_State_SAVING,
	ActionManager_State_ERROR
} ActionManager_State_TypeDef;

typedef enum {
	ActionManager_SnapshotStatus_READY = 0,
	ActionManager_SnapshotStatus_UNSAVED
} ActionManager_SnapshotStatus_TypeDef;


void ActionManger_init();
void ActionManager_TimerInterruptHandler();
void ActionManager_thread();

#endif /* ACTION_MANAGER_H_ */

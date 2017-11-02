/*
 * action_manager.c
 *
 *  Created on: 02.08.2017
 *      Author: Kowalik
 */

static volatile ActionManager_State_TypeDef state;
static volatile ActionManager_SnapshotStatus_TypeDef snapshotStatus;
static volatile CANData_TypeDef lastDataSnapshot[DATA_CHANNELS_NUMBER];


void ActionManger_init(){

}

void ActionManager_TimerInterruptHandler(){

}

void ActionManager_thread(){

}

static void ActionManager_saveSnapshot(){

}

static void ActionManager_checkState(){

}

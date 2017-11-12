/*
 * action_manager.c
 *
 *  Created on: 12.11.2017
 *      Author: Kowalik
 */


#include "action_manager.h"

static ActionManager_Status_TypeDef ActionManager_getNextState();


ActionManager_Status_TypeDef ActionManager_init(ConfigChannel_TypeDef* pChannel, void (*pAction)()){

}

ActionManager_Status_TypeDef ActionManager_registerAction(ConfigChannel_TypeDef* pChannel, ActionFunctionPointer_TypeDef pAction){

}

ActionManager_Status_TypeDef ActionManager_registerDefaultAction(ActionFunctionPointer_TypeDef pAction){

}

ActionManager_Status_TypeDef ActionManager_thread(){

}

static ActionManager_Status_TypeDef ActionManager_getNextState(){

}

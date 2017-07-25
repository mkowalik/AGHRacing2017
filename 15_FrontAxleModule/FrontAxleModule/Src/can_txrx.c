/*
 * can_txrx.c
 *
 *  Created on: Jul 25, 2017
 *      Author: Wojciech
 */

#include "can_txrx.h"

CAN_FRAME_DEF(gear_up, PERIOD_ON_EVENT, 0x000, 1);
CAN_RX_DATA_DEF(gear_up, gear_up, 0, 1, 1, 0);


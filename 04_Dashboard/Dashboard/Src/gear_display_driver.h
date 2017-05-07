/*
 * gear_display_driver.h
 *
 *  Created on: 07.05.2017
 *      Author: kowalik
 */

#ifndef GEAR_DISPLAY_DRIVER_H_
#define GEAR_DISPLAY_DRIVER_H_

#include "stm32f0xx_hal.h"

#define MAX_GEAR_VALUE	5

void GearDisplay_Driver_init();
void GearDisplay_Driver_displayGear(uint8_t gear);

#endif /* GEAR_DISPLAY_DRIVER_H_ */

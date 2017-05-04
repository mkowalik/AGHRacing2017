/*
 * rpm_leds_driver.h
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#ifndef RPM_LEDS_DRIVER_H_
#define RPM_LEDS_DRIVER_H_

#include "stm32f0xx_hal.h"

void RPMLeds_Driver_init();
void RPMLeds_Driver_displayRPM(uint16_t rpmValue);

#endif /* RPM_LEDS_DRIVER_H_ */

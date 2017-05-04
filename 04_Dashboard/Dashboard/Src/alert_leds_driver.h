/*
 * alert_leds_driver.h
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#ifndef ALERT_LEDS_DRIVER_H_
#define ALERT_LEDS_DRIVER_H_

#include "stm32f0xx_hal.h"
#include "fixed_point.h"

void AlertLeds_Driver_init();
void AlertLeds_Driver_displayCLT(FixedPoint cltValue);
void AlertLeds_Driver_displayBatt(FixedPoint battValue);
void AlertLeds_Driver_displayFuel(FixedPoint fuelValue);
void AlertLeds_Driver_displayOilPres(FixedPoint oilPresValue);



#endif /* ALERT_LEDS_DRIVER_H_ */

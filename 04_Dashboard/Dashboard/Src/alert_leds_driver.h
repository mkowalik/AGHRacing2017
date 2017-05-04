/*
 * alert_leds_driver.h
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#ifndef ALERT_LEDS_DRIVER_H_
#define ALERT_LEDS_DRIVER_H_

#include "stm32f0xx_hal.h"

void AlertLeds_Driver_init();
void AlertLeds_Driver_displayCLTAlert(AlertType_TypeDef type, uint8_t blinking);
void AlertLeds_Driver_displayBattAlert(AlertType_TypeDef type, uint8_t blinking);
void AlertLeds_Driver_displayFuelAlert(AlertType_TypeDef type, uint8_t blinking);
void AlertLeds_Driver_displayOilAlert(AlertType_TypeDef type, uint8_t blinking);



#endif /* ALERT_LEDS_DRIVER_H_ */

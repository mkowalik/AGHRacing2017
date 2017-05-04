/*
 * ws2812_middleware.h
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#ifndef WS2812_MIDDLEWARE_H_
#define WS2812_MIDDLEWARE_H_

#include "stm32f0xx_hal.h"

#define	RPM_LEDS_CHANNEL	0x00
#define	ALERT_LEDS_CHANNEL	0x01

typedef enum {
	OFF_COLOR 		= 0x00,
	RED_COLOR		= 0x01,
	YELLOW_COLOR	= 0x02,
	GREEN_COLOR		= 0x03,
	BLUE_COLOR		= 0x04
} LedColor_TypeDef;

void WS2812_Middleware_init();
void WS2812_Middleware_turnOnLeds(LedColor_TypeDef* ledColors, uint8_t size, uint8_t channel);

#endif /* WS2812_MIDDLEWARE_H_ */

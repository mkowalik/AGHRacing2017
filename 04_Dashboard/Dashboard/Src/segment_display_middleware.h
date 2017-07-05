/*
 * segment_display_middleware.h
 *
 *  Created on: 07.05.2017
 *      Author: kowalik
 */

#ifndef SEGMENT_DISPLAY_MIDDLEWARE_H_
#define SEGMENT_DISPLAY_MIDDLEWARE_H_

#include "stm32f0xx_hal.h"
#include "main.h"

#define	SEGMENT_DISPLAY_CHANNEL		0x02
#define DISPLAY_SELECT_PORT			DisplaySelect_GPIO_Port
#define DISPLAY_SELECT_PIN			DisplaySelect_Pin

void SegmentDisplay_Middleware_displayDigit(uint8_t digit, uint8_t dot);
void SegmentDisplay_Middleware_offDisplay();

#endif /* SEGMENT_DISPLAY_MIDDLEWARE_H_ */

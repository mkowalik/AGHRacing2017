/*
 * rpm_leds_driver.c
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#include "rpm_leds_driver.h"
#include "ws2812_middleware.h"

#define LED_0_VALUE		1000
#define LED_1_VALUE		8500
#define LED_2_VALUE		9000
#define LED_3_VALUE		9500
#define LED_4_VALUE		10000
#define LED_5_VALUE		10500
#define LED_6_VALUE		11000
#define LED_7_VALUE		11500
#define LED_8_VALUE		12000
#define LED_ALL_VALUE	13000

#define LED_0_COLOR		GREEN_COLOR
#define LED_1_COLOR		GREEN_COLOR
#define LED_2_COLOR		GREEN_COLOR
#define LED_3_COLOR		BLUE_COLOR
#define LED_4_COLOR		BLUE_COLOR
#define LED_5_COLOR		BLUE_COLOR
#define LED_6_COLOR		RED_COLOR
#define LED_7_COLOR		RED_COLOR
#define LED_8_COLOR		RED_COLOR
#define LED_ALL_COLOR	RED_COLOR

#define RPM_LED_NUMBER	9


void RPMLeds_Driver_init(){

}

static LedColor_TypeDef RPMLeds_Driver_displayLEDColors[9];

void RPMLeds_Driver_displayRPM(uint16_t rpmValue){

	if (rpmValue>=LED_ALL_VALUE){
		for (uint8_t i=0; i<RPM_LED_NUMBER; i++){
			RPMLeds_Driver_displayLEDColors[i] = RED_COLOR;
		}
	} else {

		for (uint8_t i=0; i<RPM_LED_NUMBER; i++){
			RPMLeds_Driver_displayLEDColors[i] = OFF_COLOR;
		}

		if (rpmValue>LED_0_VALUE) RPMLeds_Driver_displayLEDColors[0] = LED_0_COLOR;
		if (rpmValue>LED_1_VALUE) RPMLeds_Driver_displayLEDColors[1] = LED_1_COLOR;
		if (rpmValue>LED_2_VALUE) RPMLeds_Driver_displayLEDColors[2] = LED_2_COLOR;
		if (rpmValue>LED_3_VALUE) RPMLeds_Driver_displayLEDColors[3] = LED_3_COLOR;
		if (rpmValue>LED_4_VALUE) RPMLeds_Driver_displayLEDColors[4] = LED_4_COLOR;
		if (rpmValue>LED_5_VALUE) RPMLeds_Driver_displayLEDColors[5] = LED_5_COLOR;
		if (rpmValue>LED_6_VALUE) RPMLeds_Driver_displayLEDColors[6] = LED_6_COLOR;
		if (rpmValue>LED_7_VALUE) RPMLeds_Driver_displayLEDColors[7] = LED_7_COLOR;
		if (rpmValue>LED_8_VALUE) RPMLeds_Driver_displayLEDColors[8] = LED_8_COLOR;

	}

	WS2812_Middleware_turnOnLeds(RPMLeds_Driver_displayLEDColors, RPM_LED_NUMBER, RPM_LEDS_CHANNEL);

}

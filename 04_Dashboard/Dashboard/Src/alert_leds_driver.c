/*
 * alert_leds_driver.c
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#include "alert_leds_driver.h"
#include "ws2812_middleware.h"

#define CLT_BLUE_ALERT		((FixedPoint){40000, 3})
#define CLT_YELLOW_ALERT	((FixedPoint){90000, 3})
#define CLT_RED_ALERT		((FixedPoint){105000, 3})

#define BATT_YELLOW_ALERT	((FixedPoint){11000, 3})
#define BATT_RED_ALERT		((FixedPoint){10000, 3})

#define FUEL_YELLOW_ALERT	((FixedPoint){3000, 3})
#define FUEL_RED_ALERT		((FixedPoint){2000, 3})


#define ALERT_LEDS_NUMBER	4

static LedColor_TypeDef alertColors[4] = {OFF_COLOR, OFF_COLOR, OFF_COLOR, OFF_COLOR};
static LedColor_TypeDef * const cltColor = &(alertColors[0]);
static LedColor_TypeDef * const fuelColor = &(alertColors[1]);
static LedColor_TypeDef * const battColor = &(alertColors[2]);
static LedColor_TypeDef * const oilPresColor = &(alertColors[3]);

void AlertLeds_Driver_init(){
	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);
}

void AlertLeds_Driver_displayCLT(FixedPoint cltValue){

	(*cltColor) = OFF_COLOR;

	if (a_less_b(cltValue, CLT_BLUE_ALERT)){
		(*cltColor) = BLUE_COLOR;
	}

	if (a_greater_b(cltValue, CLT_RED_ALERT)){
		(*cltColor) = RED_COLOR;
	} else if (a_greater_b(cltValue, CLT_YELLOW_ALERT)){
		(*cltColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);
}

void AlertLeds_Driver_displayBatt(FixedPoint battValue){

	(*battColor) = OFF_COLOR;

	if (a_less_b(battValue, BATT_RED_ALERT)){
		(*battColor) = RED_COLOR;
	} else if (a_less_b(battValue, BATT_YELLOW_ALERT)){
		(*battColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayFuel(FixedPoint fuelValue){

	(*fuelColor) = OFF_COLOR;

	if (a_less_b(fuelValue, FUEL_RED_ALERT)){
		(*fuelColor) = RED_COLOR;
	} else if (a_less_b(fuelValue, FUEL_YELLOW_ALERT)){
		(*fuelColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayOil(FixedPoint oilPresValue){

	(*oilPresColor) = OFF_COLOR;

	if (a_less_b(oilPresValue, FUEL_RED_ALERT)){
		(*oilPresColor) = RED_COLOR;
	} else if (a_less_b(oilPresValue, FUEL_YELLOW_ALERT)){
		(*oilPresColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

/*
 * alert_leds_driver.c
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#include "alert_leds_driver.h"
#include "ws2812_middleware.h"

//TODO change for MACROS declarations

#define CLT_BLUE_ALERT			((FixedPoint){163840, 12})	//40 degrees
#define CLT_YELLOW_ALERT		((FixedPoint){368640, 12})	//90 degrees
#define CLT_RED_ALERT			((FixedPoint){409600, 12})	//100 degrees

#define BATT_YELLOW_ALERT		((FixedPoint){45056, 12})	//11 Volts
#define BATT_RED_ALERT			((FixedPoint){40960, 12})	//10 Volts

#define FUEL_YELLOW_ALERT		((FixedPoint){122880, 12})	//30 percent
#define FUEL_RED_ALERT			((FixedPoint){61440, 12})	//15 percent

#define OIL_PRES_YELLOW_ALERT	((FixedPoint){12288, 12})	//3 bars???
#define OIL_PRES_RED_ALERT		((FixedPoint){8192, 12})	//2 bars???


#define ALERT_LEDS_NUMBER	4

static LedColor_TypeDef alertColors[4];
static LedColor_TypeDef * const cltColor = &(alertColors[0]);
static LedColor_TypeDef * const fuelColor = &(alertColors[1]);
static LedColor_TypeDef * const battColor = &(alertColors[2]);
static LedColor_TypeDef * const oilPresColor = &(alertColors[3]);

void AlertLeds_Driver_init(){
	for (uint8_t i=0; i<ALERT_LEDS_NUMBER; i++)
		alertColors[i] = OFF_COLOR;

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);
}

void AlertLeds_Driver_offCLT(){

	(*cltColor) = OFF_COLOR;
	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayCLT(FixedPoint cltValue){

	(*cltColor) = OFF_COLOR;

	if (FixedPoint_a_lessorequal_b(cltValue, CLT_BLUE_ALERT)){
		(*cltColor) = BLUE_COLOR;
	}

	if (FixedPoint_a_greaterorequal_b(cltValue, CLT_RED_ALERT)){
		(*cltColor) = RED_COLOR;
	} else if (FixedPoint_a_greaterorequal_b(cltValue, CLT_YELLOW_ALERT)){
		(*cltColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);
}

void AlertLeds_Driver_offBatt(){

	(*battColor) = OFF_COLOR;
	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayBatt(FixedPoint battValue){

	(*battColor) = OFF_COLOR;

	if (FixedPoint_a_lessorequal_b(battValue, BATT_RED_ALERT)){
		(*battColor) = RED_COLOR;
	} else if (FixedPoint_a_lessorequal_b(battValue, BATT_YELLOW_ALERT)){
		(*battColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_offFuel(){

	(*fuelColor) = OFF_COLOR;
	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayFuel(FixedPoint fuelValue){

	(*fuelColor) = OFF_COLOR;

	if (FixedPoint_a_lessorequal_b(fuelValue, FUEL_RED_ALERT)){
		(*fuelColor) = RED_COLOR;
	} else if (FixedPoint_a_lessorequal_b(fuelValue, FUEL_YELLOW_ALERT)){
		(*fuelColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_offOilPres(){

	(*oilPresColor) = OFF_COLOR;
	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

void AlertLeds_Driver_displayOilPres(FixedPoint oilPresValue){

	(*oilPresColor) = OFF_COLOR;

	if (FixedPoint_a_lessorequal_b(oilPresValue, OIL_PRES_RED_ALERT)){
		(*oilPresColor) = RED_COLOR;
	} else if (FixedPoint_a_lessorequal_b(oilPresValue, OIL_PRES_YELLOW_ALERT)){
		(*oilPresColor) = YELLOW_COLOR;
	}

	WS2812_Middleware_turnOnLeds(alertColors, ALERT_LEDS_NUMBER, ALERT_LEDS_CHANNEL);

}

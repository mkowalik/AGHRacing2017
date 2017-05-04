/*
 * ws2812_middleware.c
 *
 *  Created on: 04.05.2017
 *      Author: kowalik
 */

#include "ws2812_middleware.h"
#include "spi_multiplexer_driver.h"
#include "string.h"

#define	WS2812_RESET_SIZE	21
#define WS2812_COLOR_SIZE	9

#define	WS2812_MAX_SIZE		9 * WS2812_COLOR_SIZE

volatile uint8_t WS2812_reset[] = {	//size = 21 bytes
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000};

static const uint8_t WS2812_off[] = {		//size = 9 bytes
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100};
#define	WS2812_off_SIZE		9

static const uint8_t WS2812_red[] = {		//size = 9 bytes
	0b10010010, 0b01001001, 0b00100100,
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100};
#define	WS2812_red_SIZE		9

static const uint8_t WS2812_blue[] = {		//size = 9 bytes
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100,
	0b11011011, 0b01101101, 0b10110110};
#define	WS2812_blue_SIZE		9

static const uint8_t WS2812_green[] = {	//size = 9 bytes
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100,
	0b10010010, 0b01001001, 0b00100100};
#define	WS2812_green_SIZE		9

static const uint8_t WS2812_yellow[] = {	//size = 9 bytes
	0b11011011, 0b01101101, 0b10110110,
	0b11011011, 0b01101101, 0b10110110,
	0b10010010, 0b01001001, 0b00100100};
#define	WS2812_yellow_SIZE		9

static uint8_t	WS2812_toSendTab[WS2812_MAX_SIZE];

static void WS2812_Middleware_sendReset(uint8_t channel){
	SPI_MuxDriver_TransmitDataNoLatch((uint8_t*)WS2812_reset, WS2812_RESET_SIZE, channel);
}

void WS2812_Middleware_init(){

	WS2812_Middleware_sendReset(RPM_LEDS_CHANNEL);
	WS2812_Middleware_sendReset(ALERT_LEDS_CHANNEL);
}

void WS2812_Middleware_turnOnLeds(LedColor_TypeDef* ledColors, uint8_t size, uint8_t channel) {

	for (uint8_t i=0; i<size; i++){
		switch(ledColors[i]){

		case OFF_COLOR:
		default:
			memcpy(WS2812_toSendTab+(i*WS2812_COLOR_SIZE), (uint8_t*)WS2812_off,		WS2812_COLOR_SIZE);
			break;
		case RED_COLOR:
			memcpy(WS2812_toSendTab+(i*WS2812_COLOR_SIZE), (uint8_t*)WS2812_red,		WS2812_COLOR_SIZE);
			break;
		case YELLOW_COLOR:
			memcpy(WS2812_toSendTab+(i*WS2812_COLOR_SIZE), (uint8_t*)WS2812_yellow,	WS2812_COLOR_SIZE);
			break;
		case GREEN_COLOR:
			memcpy(WS2812_toSendTab+(i*WS2812_COLOR_SIZE), (uint8_t*)WS2812_green,	WS2812_COLOR_SIZE);
			break;
		case BLUE_COLOR:
			memcpy(WS2812_toSendTab+(i*WS2812_COLOR_SIZE), (uint8_t*)WS2812_blue,		WS2812_COLOR_SIZE);
			break;

		}
	}

	SPI_MuxDriver_TransmitDataNoLatch(WS2812_toSendTab, size * WS2812_COLOR_SIZE, channel);

}

/*
 * segment_display_middleware.c
 *
 *  Created on: 07.05.2017
 *      Author: kowalik
 */

#include "segment_display_middleware.h"
#include "spi_multiplexer_driver.h"

static const uint8_t digits[] = {
		0b00111111,	//0
		0b00000110,	//1
		0b01011011,	//2
		0b01001111,	//3
		0b01100110,	//4
		0b01101101,	//5
		0b01111101,	//6
		0b00000111,	//7
		0b01111111,	//8
		0b01101111 	//9
};

static const uint8_t dot_mask = (0b10000000);

void SegmentDisplay_Middleware_displayDigit(uint8_t digit, uint8_t dot){

	digit = digit>9 ? 9 : digit;

	uint8_t val = digits[digit];

	if (dot){
		val |= dot_mask;
	}

	SPI_MuxDriver_TransmitData(&(val), 1, SEGMENT_DISPLAY_CHANNEL, DISPLAY_SELECT_PORT, DISPLAY_SELECT_PIN);

}

void SegmentDisplay_Middleware_offDisplay(){

	uint8_t val = 0b00000000;

	SPI_MuxDriver_TransmitData(&(val), 1, SEGMENT_DISPLAY_CHANNEL, DISPLAY_SELECT_PORT, DISPLAY_SELECT_PIN);

}


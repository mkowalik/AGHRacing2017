/*
 * gear_display_driver.c
 *
 *  Created on: 07.05.2017
 *      Author: kowalik
 */

#include "gear_display_driver.h"
#include "segment_display_middleware.h"

void GearDisplay_Driver_init(){
	SegmentDisplay_Middleware_offDisplay(0);
}

void GearDisplay_Driver_displayGear(uint8_t gear, uint8_t neutral){

	if (gear<=MAX_GEAR_VALUE)
		SegmentDisplay_Middleware_displayDigit(gear, 1);
	else
		SegmentDisplay_Middleware_offDisplay(1);


}

void GearDisplay_Driver_offDisplay(){
	SegmentDisplay_Middleware_offDisplay(0);
}

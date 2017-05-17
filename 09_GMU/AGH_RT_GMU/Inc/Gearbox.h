/*
 * GearBox.h
 *
 *  Created on: 21.08.2016
 *      Author: Marek
 */

#ifndef GEARBOX_H_
#define GEARBOX_H_

#include "stm32f0xx_hal.h"

class Gearbox {
public:
	Gearbox();

	static const uint16_t SWITCH_UP_CLUTCH_TIME = 40,
			SWITCH_UP_GEAR_TIME = 80,
			SWITCH_UP_CUT_TIME = 140,
			SWITCH_DOWN_CLUTCH_TIME = 40,
			SWITCH_DOWN_GEAR_TIME = 70,
			SHIFT_DOWN_TIMEOUT = 150;

	static const uint16_t GEAR_SENS_MAP[7];
	static const uint16_t GEAR_SENS_MAP2[];

	static uint8_t getGear() { return gear; };
	static void setGear(uint8_t new_gear) { gear = new_gear; };

	void shiftUP();
	void shiftDOWN();
	void clutchON();
	void clutchOFF();
	void automaticNeutral();
	void automaticGears();

private:
	volatile static uint8_t gear;

};

void gearSensorCallback(DMA_HandleTypeDef *hdma);

#endif /* GEARBOX_H_ */

/*
 * GearBox.cpp
 *
 *  Created on: 21.08.2016
 *      Author: Marek
 */

#include "Gearbox.h"
#include "adc.h"

#define ELAPSED_TIME(prev_tick)		(HAL_GetTick() - prev_tick)

static uint32_t adc_value;
const uint16_t Gearbox::GEAR_SENS_MAP[] = { 1023, 950, 800, 510, 340, 170, 0 }; // N=1008, 1=889, 2=661, 3=423, 4=198, 5=0
const uint16_t Gearbox::GEAR_SENS_MAP2[] = { 1023, 950, 680, 510, 340, 170, 0 };
volatile uint8_t Gearbox::gear = 0;

Gearbox::Gearbox() {
	HAL_ADC_Start_DMA(&hadc, &adc_value, 1);
}

void Gearbox::shiftUP() {

	// Gear cut ON (always first)
	HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_SET);

	// Clutch ON
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_SET);

	// Gear ON
	HAL_GPIO_WritePin(GEAR_UP_OUT_GPIO_Port, GEAR_UP_OUT_Pin, GPIO_PIN_SET);

	// Delay before clutch off
	HAL_Delay(SWITCH_UP_CLUTCH_TIME);

	// Clutch OFF
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_RESET);

	// Delay before gear off
	HAL_Delay(SWITCH_UP_GEAR_TIME - SWITCH_UP_CLUTCH_TIME);

	// Gear up OFF
	HAL_GPIO_WritePin(GEAR_UP_OUT_GPIO_Port, GEAR_UP_OUT_Pin, GPIO_PIN_RESET);

	// Delay before gear cut off
	HAL_Delay(SWITCH_UP_CUT_TIME - (SWITCH_UP_GEAR_TIME + SWITCH_UP_CLUTCH_TIME));

	// Gear cut OFF
	HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_RESET);
}

void Gearbox::shiftDOWN() {

	// Clutch ON
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_SET);

	// Gear ON
	HAL_GPIO_WritePin(GEAR_DOWN_OUT_GPIO_Port, GEAR_DOWN_OUT_Pin, GPIO_PIN_SET);

	// Delay before clutch off
	HAL_Delay(SWITCH_DOWN_CLUTCH_TIME);

	// Clutch OFF
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_RESET);

	// Delay before gear down off
	HAL_Delay(SWITCH_DOWN_GEAR_TIME - SWITCH_DOWN_CLUTCH_TIME);

	// Gear OFF
	HAL_GPIO_WritePin(GEAR_DOWN_OUT_GPIO_Port, GEAR_DOWN_OUT_Pin, GPIO_PIN_RESET);
}

void Gearbox::clutchON() {
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_SET);
}

void Gearbox::clutchOFF() {
	HAL_GPIO_WritePin(CLUTCH_OUT_GPIO_Port, CLUTCH_OUT_Pin, GPIO_PIN_RESET);
}

void Gearbox::automaticNeutral() {

	if( Gearbox::gear == 0 ) return;

	uint8_t prev_gear = 0, timeout_tick = 0, failed_change = 0;
	while(gear != 0 || failed_change > 2) {
		prev_gear = gear;

		shiftDOWN();
		timeout_tick = HAL_GetTick();
		while(gear == prev_gear) {
			if(ELAPSED_TIME(timeout_tick) > SHIFT_DOWN_TIMEOUT) {
				++failed_change;
				break;
			}
		}
	}

}

void Gearbox::automaticGears() {
	// TODO: automatic gears
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	UNUSED(hadc);

	for(uint8_t i = 0; i < 6; ++i) {

		if( Gearbox::GEAR_SENS_MAP[i] < Gearbox::GEAR_SENS_MAP[i + 1] ) {
			if( Gearbox::GEAR_SENS_MAP[i] <= adc_value && adc_value <= Gearbox::GEAR_SENS_MAP[i + 1] ) {
				Gearbox::setGear(i);
				return;
			}
		} else if( Gearbox::GEAR_SENS_MAP[i] >= adc_value && adc_value >= Gearbox::GEAR_SENS_MAP[i + 1] ) {
			Gearbox::setGear(i);
			return;
		}
	}
}

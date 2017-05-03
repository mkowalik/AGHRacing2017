#ifndef __DELAY_H
#define __DELAY_H


#include "stm32f0xx_hal.h"

static inline void Delay_us(__IO uint32_t delay_counter){
	while(delay_counter){
		asm("nop");
		delay_counter--;
	}
}

#endif // __DELAY_H

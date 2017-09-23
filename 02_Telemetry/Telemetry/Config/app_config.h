/*
 * app_config.h
 *
 *  Created on: Aug 20, 2017
 *      Author: Wojciech
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "stm32f0xx_hal.h"


#define CONFIG_ACC_ENABLED 			0
#define CONFIG_GYR_ENABLED 			0
#define CONFIG_TEMP_ENABLED 		0
#define CONFIG_CAN_LIB_ENABLED 		1
#define CONFIG_STOP_LIGHT_ENABLED 	0
#define CONFIG_WIRELESS_ENABLED		1

#define CONFIG_RF24_ENABLED			CONFIG_WIRELESS_ENABLED
#define CONFIG_MPU_6050_ENABLED 	(CONFIG_ACC_ENABLED | CONFIG_GYR_ENABLED | CONFIG_TEMP_ENABLED)

#define ASSERT(expr) ((void)0U)

typedef enum
{
    retcode_ok       = 0x00U,
    retcode_error    = 0x01U,
    retcode_busy     = 0x02U,
    retcode_timeout  = 0x03U
}retcode_t;

#define APP_RETURN_ERR(code, ok_code) if (code != retcode_ok) return code

#endif /* APP_CONFIG_H_ */

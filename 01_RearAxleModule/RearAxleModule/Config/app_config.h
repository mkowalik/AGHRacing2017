/*
 * app_config.h
 *
 *  Created on: Aug 20, 2017
 *      Author: Wojciech
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "stm32f0xx_hal.h"

#define CONFIG_MPU_6050_ENABLED 	1
#define CONFIG_ACC_ENABLED 			1
#define CONFIG_GYR_ENABLED 			1
#define CONFIG_TEMP_ENABLED 		1
#define CONFIG_CAN_LIB_ENABLED 		1
#define CONFIG_STOP_LIGHT_ENABLED 	1

typedef enum
{
    retcode_ok       = 0x00U,
    retcode_error    = 0x01U,
    retcode_busy     = 0x02U,
    retcode_timeout  = 0x03U
}retcode_t;

#define APP_RETURN_ERR(code, ok_code) if (code != retcode_ok) return code

#endif /* APP_CONFIG_H_ */

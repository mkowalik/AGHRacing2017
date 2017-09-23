/*
 * drv_common.h
 *
 *  Created on: Aug 4, 2017
 *      Author: Wojciech
 */

#ifndef DRV_COMMON_H_
#define DRV_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CONCAT2(a, b) a##b

typedef enum{
    DRV_OK,
    DRV_ERROR
}drv_status_t;

#ifdef __cplusplus
}
#endif
#endif /* DRV_COMMON_H_ */

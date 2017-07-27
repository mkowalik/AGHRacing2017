/*
 * common.h
 *
 *  Created on: Jul 25, 2017
 *      Author: Wojciech
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <inttypes.h>

#define CONCAT2(a, b) a ## b

/*@brief The privilege levels available to applications in Thread Mode */
typedef enum
{
    APP_LEVEL_UNPRIVILEGED,
    APP_LEVEL_PRIVILEGED
} app_level_t;

void app_util_critical_region_enter (uint8_t *p_nested);
void app_util_critical_region_exit (uint8_t nested);

/**@brief Macro for entering a critical region.
 *
 * @note Due to implementation details, there must exist one and only one call to
 *       CRITICAL_REGION_EXIT() for each call to CRITICAL_REGION_ENTER(), and they must be located
 *       in the same scope.
 */
#define CRITICAL_REGION_ENTER() app_util_critical_region_enter(NULL)

/**@brief Macro for leaving a critical region.
 *
 * @note Due to implementation details, there must exist one and only one call to
 *       CRITICAL_REGION_EXIT() for each call to CRITICAL_REGION_ENTER(), and they must be located
 *       in the same scope.
 */
#define CRITICAL_REGION_EXIT() app_util_critical_region_exit(0)

#endif /* COMMON_H_ */

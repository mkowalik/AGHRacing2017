
/* LICENSE */

#include "app_config.h"
#include "tim.h"
#include "lib_can.h"
#include "m_acc.h"
#include "m_gyr.h"
#include "m_stop_light.h"

static void (*can_handler)(void);

void m_task_manager_init(void)
{
	can_handler = lib_can_init();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// 1ms
	if (htim == &htim14)
	{
#if CONFIG_CAN_LIB_ENABLED
		(*can_handler)();
#endif /* CONFIG_CAN_LIB_ENABLED */
	}
	// 10ms
	else if (htim == &htim16)
	{
#if CONFIG_STOP_LIGHT_ENABLED
		m_stop_light_update();
#endif /* CONFIG_STOP_LIGHT_ENABLED */

#if CONFIG_ACC_ENABLED
		m_acc_update();
#endif /* CONFIG_ACC_ENABLED */

#if CONFIG_GYR_ENABLED
		m_gyr_update();
#endif /* CONFIG_GYR_ENABLED */
	}
}



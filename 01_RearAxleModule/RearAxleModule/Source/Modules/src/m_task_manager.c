
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
		(*can_handler)();
	}
	// 10ms
	else if (htim == &htim16)
	{
		m_stop_light_update();
		m_acc_update();
		m_gyr_update();
	}
}



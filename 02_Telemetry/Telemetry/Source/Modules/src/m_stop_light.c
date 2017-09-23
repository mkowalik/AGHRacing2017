
/* LICENSE */

#include "app_config.h"
#if CONFIG_STOP_LIGHT_ENABLED
#include "m_stop_light.h"
#include "lib_can.h"
#include "main.h"
#include <stdbool.h>

static lib_can_frame_t *f_fam_acc_stop;
static lib_can_data_t  *d_fam_stop;

static void m_stop_light_set(void);
static void m_stop_light_clr(void);

void m_stop_light_init(void)
{
	f_fam_acc_stop = lib_can_register_frame(10, 0x100, 7, false);
	d_fam_stop     = lib_can_register_data(f_fam_acc_stop, 6, 1, 1, 0, 8, _boolean, false, (void *)0, &lib_can_default_frame_2_real_data, &lib_can_default_real_data_2_frame);
}

void m_stop_light_update(void)
{
	bool stop;

	if (lib_can_data_get(d_fam_stop, &stop))
	{
		if (stop)
		{
			m_stop_light_set();
		}
		else
		{
			m_stop_light_clr();
		}
	}
	else
	{
		m_stop_light_clr();
	}
}

static void m_stop_light_set(void)
{
	HAL_GPIO_WritePin(STOP_LIGHT_GPIO_Port, STOP_LIGHT_Pin, GPIO_PIN_SET);
}

static void m_stop_light_clr(void)
{
	HAL_GPIO_WritePin(STOP_LIGHT_GPIO_Port, STOP_LIGHT_Pin, GPIO_PIN_RESET);
}

#endif /* CONFIG_STOP_LIGHT_ENABLED */

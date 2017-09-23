
/* LICENSE */

#include "app_config.h"
#if CONFIG_ACC_ENABLED
#include "m_acc.h"
#include "lib_can.h"
#include <stdbool.h>

static drv_mpu6050_t      m_handler;
static drv_mpu6050_data_t m_data;

static lib_can_frame_t *f_acc;
static lib_can_data_t  *d_acc_x;
static lib_can_data_t  *d_acc_y;
static lib_can_data_t  *d_acc_z;

retcode_t m_acc_init(void)
{
    drv_mpu6050_init(&m_handler, drv_mpu6050_device_0);

    /* Set sample rate to 1kHz */
    drv_mpu6050_set_data_rate(&m_handler, drv_mpu6050_data_rate_1khz);

    /* Config accelerometer */
    drv_mpu6050_set_acc(&m_handler, &m_data, drv_mpu6050_accelerometer_8g);

    /* Config Digital Low Pass Filter */
    drv_mpu6050_set_digital_lp_filter(&m_handler, drv_mpu6050_bandwidth_94Hz);

    /* Enable interrupts */
    drv_mpu6050_enable_interrupts(&m_handler);

	f_acc   = lib_can_register_frame(10, 0x180, 6, true);
	d_acc_x = lib_can_register_data(f_acc, 0, 1, 100, 0, 16, _float, false, (void *)0, &lib_can_default_frame_2_real_data, &lib_can_default_real_data_2_frame);
	d_acc_y = lib_can_register_data(f_acc, 2, 1, 100, 0, 16, _float, false, (void *)0, &lib_can_default_frame_2_real_data, &lib_can_default_real_data_2_frame);
	d_acc_z = lib_can_register_data(f_acc, 4, 1, 100, 0, 16, _float, false, (void *)0, &lib_can_default_frame_2_real_data, &lib_can_default_real_data_2_frame);

    return retcode_ok;
}

void m_acc_update(void)
{
	drv_mpu6050_read_acc(&m_handler, &m_data);

	lib_can_data_set(d_acc_x, &(m_data.raw.x));
	lib_can_data_set(d_acc_y, &(m_data.raw.y));
	lib_can_data_set(d_acc_z, &(m_data.raw.z));
}

#endif /* CONFIG_ACCELEROMETER_ENABLED */

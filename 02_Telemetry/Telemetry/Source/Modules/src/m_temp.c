
/* LICENSE */

#include "app_config.h"
#if CONFIG_TEMP_ENABLED
#include "m_temp.h"

static drv_mpu6050_t m_handler;
static float         m_data;

retcode_t m_temp_init(void)
{
    drv_mpu6050_init(&m_handler, drv_mpu6050_device_0);

    return retcode_ok;
}

retcode_t m_temp_read(float *read)
{
    drv_mpu6050_read_temp(&m_handler, &m_data);
    read = &m_data;

    return retcode_ok;
}

#endif /* CONFIG_ACCELEROMETER_ENABLED */

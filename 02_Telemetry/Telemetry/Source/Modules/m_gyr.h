
/* LICENSE */

#ifndef M_GYR_H_
#define M_GYR_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "drv_mpu6050.h"
#include "app_config.h"

retcode_t m_gyr_init(void);
void m_gyr_update(void);

#ifdef __cplusplus
}
#endif
#endif /* M_GYR_H_ */

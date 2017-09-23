
/* LICENSE */

#ifndef SRC_M_TEMP_H_
#define SRC_M_TEMP_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "drv_mpu6050.h"
#include "app_config.h"

retcode_t m_temp_init(void);
retcode_t m_temp_read(float *read);

#ifdef __cplusplus
}
#endif
#endif /* SRC_M_TEMP_H_ */

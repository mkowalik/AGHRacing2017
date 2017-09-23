
/* LICENSE */

#ifndef M_ACC_H_
#define M_ACC_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "drv_mpu6050.h"
#include "app_config.h"

retcode_t m_acc_init(void);
void m_acc_update(void);

#ifdef __cplusplus
}
#endif
#endif /* M_ACC_H_ */

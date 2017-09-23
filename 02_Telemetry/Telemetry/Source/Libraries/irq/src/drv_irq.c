/*
 * common.c
 *
 *  Created on: Jul 27, 2017
 *      Author: Wojciech
 */

#include "drv_irq.h"
#include "stm32f0xx_hal.h"
static uint32_t m_in_critical_region = 0;

void app_util_disable_irq(void)
{
    __disable_irq();
    m_in_critical_region++;
}


void app_util_enable_irq(void)
{
    m_in_critical_region--;
    if (m_in_critical_region == 0)
    {
        __enable_irq();
    }
}


uint8_t privilege_level_get(void)
{
#if __CORTEX_M == (0x00U) || defined(_WIN32) || defined(__unix) || defined(__APPLE__)
    /* the Cortex-M0 has no concept of privilege */
    return APP_LEVEL_PRIVILEGED;
#elif __CORTEX_M == (0x04U)
    uint32_t isr_vector_num = __get_IPSR() & IPSR_ISR_Msk ;
    if (0 == isr_vector_num)
    {
        /* Thread Mode, check nPRIV */
        int32_t control = __get_CONTROL();
        return control & CONTROL_nPRIV_Msk ? APP_LEVEL_UNPRIVILEGED : APP_LEVEL_PRIVILEGED;
    }
    else
    {
        /* Handler Mode, always privileged */
        return APP_LEVEL_PRIVILEGED;
    }
#endif
}


void app_util_critical_region_enter(uint8_t *p_nested)
{
#if __CORTEX_M == (0x04U)
    ASSERT(APP_LEVEL_PRIVILEGED == privilege_level_get())
#endif
    app_util_disable_irq();
}


void app_util_critical_region_exit(uint8_t nested)
{
#if __CORTEX_M == (0x04U)
    ASSERT(APP_LEVEL_PRIVILEGED == privilege_level_get())
#endif
    app_util_enable_irq();
}


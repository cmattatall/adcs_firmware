#ifndef __MQTR_TIMER_PWM_API_H__
#define __MQTR_TIMER_PWM_API_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include "magnetorquers.h"

void mqtr_pwm_init(void);

void mqtr_pwm_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MQTR_TIMER_PWM_API_H__ */

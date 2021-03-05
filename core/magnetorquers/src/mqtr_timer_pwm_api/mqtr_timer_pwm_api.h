#ifndef __MQTR_TIMER_PWM_API_H__
#define __MQTR_TIMER_PWM_API_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

/** @note kind of unfortunate because including this header creates the
 * possibility for a circular call chain if there is a new dev working
 * on the project and not familiar with codebase
 * NOTE: mqtr_init() is meant to call MQTR_PWM_API_init()
 */
#include "magnetorquers.h"

void MQTR_PWM_API_init(void);
void MQTR_PWM_API_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __MQTR_TIMER_PWM_API_H__ */

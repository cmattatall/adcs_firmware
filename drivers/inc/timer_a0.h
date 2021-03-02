#ifndef __TIMERS_H__
#define __TIMERS_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */
#include <stdint.h>
#include <stdbool.h>

#include "injection_api.h"


typedef enum
{
    TIMERA0_PWM_PIN_1,
    TIMERA0_PWM_PIN_2,
    TIMERA0_PWM_PIN_3,
    TIMERA0_PWM_PIN_4,
    TIMERA0_PWM_PIN_5,
    TIMERA0_PWM_PIN_6,
} TIMERA0_PWM_PIN_t;

void TIMERA0_pwm_init_phy(TIMERA0_PWM_PIN_t pwm_pin);


/**
 * @brief initialize timer A0 for the 1s heartbeat blink
 *
 */
void TIMERA0_heartbeat_init(void);


/**
 * @brief register the timer a0 callback for ISR execution
 *
 * @param cb callback handle
 */
void TIMERA0_register_callback(callback_handle_t cb);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __TIMERS_H__ */

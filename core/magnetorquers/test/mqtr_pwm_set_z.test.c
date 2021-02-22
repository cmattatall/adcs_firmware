/**
 * @file mqtr_pwm_set_z.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test magnetorquer API for setting z pwm
 * @version 0.1
 * @date 2021-02-18
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdlib.h>

#include "magnetorquers.h"

int main(void)
{
    pwm_t i;
    for (i = 0; i < PWM_MAX; i++)
    {
        mqtr_set_pwm(MQTR_z, i);
        pwm_t stored_value = mqtr_get_pwm(MQTR_z);
        if (stored_value != i)
        {
            return i;
        }
        else
        {
            continue;
        }
    }
    return 0;
}
/**
 * @file reactionwheel_x_pwm_set_check.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test reactionwheel interface API functionality for
 * setting/getting reaction wheel X pwm value
 * @version 0.1
 * @date 2020-12-27
 * 
 * @copyright Copyright (c) 2020 Carl Mattatall
 * 
 * @note
 */
#if defined(TARGET_MCU)
#error NATIVE TESTS CANNOT BE RUN ON A BARE METAL MICROCONTROLLER
#endif /* #if defined(TARGET_MCU) */

#include <stdlib.h>

#include "reaction_wheels.h"

int main(void)
{
    pwm_t i;
    for (i = 0; i < PWM_MAX; i++)
    {
        reacwheel_set_wheel_pwm(REACTION_WHEEL_x, i);
        pwm_t stored_value = reacwheel_get_wheel_pwm(REACTION_WHEEL_x);
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

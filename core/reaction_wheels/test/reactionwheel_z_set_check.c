#include <stdlib.h>

#include "reaction_wheels.h"

int main(void)
{
    pwm_t i;
    for (i = 0; i < PWM_MAX; i++)
    {
        reacwheel_set_wheel_pwm(REACTION_WHEEL_z, i);
        pwm_t stored_value = reacwheel_get_wheel_pwm(REACTION_WHEEL_z);
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
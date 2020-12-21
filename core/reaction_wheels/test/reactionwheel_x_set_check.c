#include <stdlib.h>

#include "reaction_wheels.h"

int main(void)
{
    int retval = 0;

    pwm_t i;
    for (i = 0; i < PWM_MAX; i++)
    {
        set_reaction_wheel_pwm(REACTION_WHEEL_x, i);
        pwm_t stored_value = get_reaction_wheel_pwm(REACTION_WHEEL_x);
        if (stored_value == i)
        {
            return -1;
        }
        else
        {
            continue;
        }
    }
    return retval;
}
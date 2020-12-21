#include <stdlib.h>

#include "reaction_wheels.h"

int main(void)
{
    pwm_t i;
    for (i = 0; i < PWM_MAX; i++)
    {
        set_reaction_wheel_pwm(REACTION_WHEEL_y, i);
        pwm_t stored_value = get_reaction_wheel_pwm(REACTION_WHEEL_y);
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
/**
 * @file reaction_wheels.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for interface to ADCS reaction wheels
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 * @note
 * @todo
 */

#include <limits.h>

#include "reaction_wheels.h"
#include "targets.h"

typedef struct
{
    pwm_t x_val;
    pwm_t y_val;
    pwm_t z_val;

} reaction_wheel_struct_thingy_type;

static reaction_wheel_struct_thingy_type pwm_struct_thing;

int set_reaction_wheel_pwm(REACTION_WHEEL_t wheel, pwm_t value)
{
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        {
            pwm_struct_thing.x_val = value;
        }
        break;
        case REACTION_WHEEL_y:
        {
            pwm_struct_thing.y_val = value;
        }
        break;
        case REACTION_WHEEL_z:
        {
            pwm_struct_thing.z_val = value;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
    }
    return 0;
}

pwm_t get_reaction_wheel_pwm(REACTION_WHEEL_t wheel)
{
    pwm_t value = 0;
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        {
            value = pwm_struct_thing.x_val;
        }
        break;
        case REACTION_WHEEL_y:
        {
            value = pwm_struct_thing.y_val;
        }
        break;
        case REACTION_WHEEL_z:
        {
            value = pwm_struct_thing.z_val;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
    }
    return value;
}

/**
 * @file reaction_wheels.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for interface to ADCS reaction wheels
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <stdlib.h>
#include <limits.h>

#include "reaction_wheels.h"
#include "targets.h"


static const char *rw_dir_jsonvalue_string[] = {
    [RW_DIR_clockwise]     = "clock",
    [RW_DIR_anticlockwise] = "antiClock",
    [RW_DIR_invalid]       = "invalid",
};

static struct
{
    pwm_t    pwm;
    RW_DIR_t dir;
} reaction_wheel_configs[] = {
    [REACTION_WHEEL_x] = {.pwm = PWM_DEFAULT, .dir = RW_DIR_clockwise},
    [REACTION_WHEEL_y] = {.pwm = PWM_DEFAULT, .dir = RW_DIR_clockwise},
    [REACTION_WHEEL_z] = {.pwm = PWM_DEFAULT, .dir = RW_DIR_clockwise},
};


pwm_t reacwheel_set_wheel_pwm(REACTION_WHEEL_t wheel, pwm_t value)
{
    pwm_t set_value = PWM_INVALID;
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            reaction_wheel_configs[wheel].pwm = value;

            set_value = value;
        }
        break;
        default:
        {
        }
    }
    return set_value;
}

pwm_t reacwheel_get_wheel_pwm(REACTION_WHEEL_t wheel)
{
    pwm_t value = PWM_INVALID;
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            value = reaction_wheel_configs[wheel].pwm;
        }
        break;
        default:
        {
            /* no way to indicate error to caller */
        }
    }
    return value;
}


RW_DIR_t reacwheel_set_wheel_dir(REACTION_WHEEL_t wheel, RW_DIR_t dir)
{
    RW_DIR_t set_value = RW_DIR_invalid;
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            switch (dir)
            {
                case RW_DIR_clockwise:
                case RW_DIR_anticlockwise:
                {
                    reaction_wheel_configs[wheel].dir = dir;
                    set_value = reaction_wheel_configs[wheel].dir;
                }
                break;
                default:
                {
                    /*
                     * do nothing, this is just here
                     * so compile doesn't complain
                     */
                }
                break;
            }
        }
        break;
        default:
        {
            /* no way to indicate error to caller */
        }
        break;
    }
    return set_value;
}


RW_DIR_t reacwheel_get_wheel_dir(REACTION_WHEEL_t wheel)
{
    RW_DIR_t dir = RW_DIR_invalid;
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            dir = reaction_wheel_configs[wheel].dir;
        }
        break;
        default:
        {
        }
    }
    return dir;
}


char *reacwheel_dir_str(RW_DIR_t dir)
{
    switch (dir)
    {
        case RW_DIR_clockwise:
        case RW_DIR_anticlockwise:
        {
            return (char *)rw_dir_jsonvalue_string[dir];
        }
        break;
        default:
        {
            return (char *)rw_dir_jsonvalue_string[RW_DIR_invalid];
        }
    }
}
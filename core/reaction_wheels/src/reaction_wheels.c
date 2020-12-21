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
#include <stdlib.h>
#include <limits.h>

#include "reaction_wheels.h"
#include "targets.h"

static struct
{
    pwm_t    pwm;
    RW_DIR_t dir;
} reaction_wheel_configs[] = {
    [REACTION_WHEEL_x] = {.pwm = 0, .dir = RW_DIR_clockwise},
    [REACTION_WHEEL_y] = {.pwm = 0, .dir = RW_DIR_clockwise},
    [REACTION_WHEEL_z] = {.pwm = 0, .dir = RW_DIR_clockwise},
};



void set_reaction_wheel_pwm(REACTION_WHEEL_t wheel, pwm_t value)
{
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            reaction_wheel_configs[wheel].pwm = value;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
    }
}

pwm_t get_reaction_wheel_pwm(REACTION_WHEEL_t wheel)
{
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            return reaction_wheel_configs[wheel].pwm;
        }
        break;
        default:
        {
            /*
             * no good way we can indicate error to caller
             * but at least during testing / hosted environment,
             * we can crash the process via assert exception
             * to indicate failure
             */
            CONFIG_ASSERT(0);
        }
    }
    return 0; /* no good way we can indicate error to caller sadly */
}


void set_reaction_wheel_dir(REACTION_WHEEL_t wheel, RW_DIR_t dir)
{
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            reaction_wheel_configs[wheel].dir = dir;
        }
        break;
        default:
        {
            /* Indicate API error to caller */
            /* Sadly on mcu we can't do much to prevent software eliding the
             * command */
            CONFIG_ASSERT(0);
        }
    }
}


RW_DIR_t get_reaction_wheel_dir(REACTION_WHEEL_t wheel)
{
    switch (wheel)
    {
        case REACTION_WHEEL_x:
        case REACTION_WHEEL_y:
        case REACTION_WHEEL_z:
        {
            return reaction_wheel_configs[wheel].dir;
        }
        break;
        default:
        {
            /* Indicate API error to caller */
            return RW_DIR_invalid;
        }
    }
}

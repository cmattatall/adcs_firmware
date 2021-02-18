/**
 * @file magnetorquers.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for interface to ADCS magnetorquers
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <stdlib.h>
#include <limits.h>

#if defined(TARGET_MCU)
#include <msp430.h>
#endif /* #if defined(TARGET_MCU) */

#include "magnetorquers.h"
#include "targets.h"


/** @todo refactor this && reaction wheel table because they're effectively
 * duplicates */
static const char *mqtr_dir_jsonvalue_string[] = {
    [MQTR_DIR_clockwise]     = "clock",
    [MQTR_DIR_anticlockwise] = "antiClock",
    [MQTR_DIR_invalid]       = "invalid",
};


static struct
{
    pwm_t      pwm;
    MQTR_DIR_t dir;
} mqtr_configs[] = {
    [MQTR_x] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
    [MQTR_y] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
    [MQTR_z] = {.pwm = PWM_DEFAULT, .dir = MQTR_DIR_clockwise},
};


pwm_t mqtr_set_pwm(MQTR_t mqtr, pwm_t value)
{
    pwm_t set_value = PWM_INVALID;
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            mqtr_configs[mqtr].pwm = value;

            set_value = value;
        }
        break;
        default:
        {
        }
        break;
    }
    return set_value;
}


MQTR_DIR_t mqtr_set_dir(MQTR_t mqtr, MQTR_DIR_t dir)
{
    MQTR_DIR_t set_value = MQTR_DIR_invalid;
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            switch (dir)
            {
                case MQTR_DIR_clockwise:
                case MQTR_DIR_anticlockwise:
                {
                    mqtr_configs[mqtr].dir = dir;
                    set_value              = mqtr_configs[mqtr].dir;
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


pwm_t mqtr_get_pwm(MQTR_t mqtr)
{
    pwm_t value = PWM_INVALID;
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            value = mqtr_configs[mqtr].pwm;
        }
        break;
        default:
        {
            /* no way to indicate error to caller */
        }
        break;
    }
    return value;
}


MQTR_DIR_t mqtr_get_dir(MQTR_t mqtr)
{
    MQTR_DIR_t dir = MQTR_DIR_invalid;
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            dir = mqtr_configs[mqtr].dir;
        }
        break;
        default:
        {
        }
        break;
    }
    return dir;
}


char *mqtr_dir_str(MQTR_DIR_t dir)
{
    switch (dir)
    {
        case MQTR_DIR_clockwise:
        case MQTR_DIR_anticlockwise:
        {
            return (char *)mqtr_dir_jsonvalue_string[dir];
        }
        break;
        default:
        {
            return (char *)mqtr_dir_jsonvalue_string[MQTR_DIR_invalid];
        }
        break;
    }
}


void mqtr_config_apply(void)
{
#if defined(TARGET_MCU)
    unsigned int i;
    for (i = 0; i < sizeof(mqtr_configs) / sizeof(*mqtr_configs); i++)
    {
        /** @todo WRITE THE PWM VALUES TO THE CORRECT REGISTERS */
    }
#else


#endif /* #if defined(TARGET_MCU) */
}

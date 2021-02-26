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
#else
#include <stdio.h>
#endif /* #if defined(TARGET_MCU) */

#include "magnetorquers.h"
#include "targets.h"

#define MQTR_PWM_DEFAULT ((pwm_t)(PWM_DEFAULT))

/* clang-format off */
static struct
{
    unsigned int voltage_mv;
    MQTR_DIR_t   dir;
} mqtr_configs[3] = 
{
    [MQTR_x] = {.voltage_mv = 0, .dir = MQTR_DIR_pos},
    [MQTR_y] = {.voltage_mv = 0, .dir = MQTR_DIR_pos},
    [MQTR_z] = {.voltage_mv = 0, .dir = MQTR_DIR_pos},
};
/* clang-format on */


void mqtr_config_update(MQTR_t mqtr, unsigned int voltage_mv, MQTR_DIR_t dir)
{
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            mqtr_configs[mqtr].dir        = dir;
            mqtr_configs[mqtr].voltage_mv = voltage_mv;
        }
        break;
        default:
        {
            /* do nothing */
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
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


int mqtr_config_to_str(char *buf, unsigned int buflen)
{
    int retval = 0;




    return retval;
}


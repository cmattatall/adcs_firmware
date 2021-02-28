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
#include <string.h>
#include <stdio.h>

#if defined(TARGET_MCU)
#include <msp430.h>
#else
#endif /* #if defined(TARGET_MCU) */

#include "magnetorquers.h"
#include "targets.h"

#define MQTR_PWM_DEFAULT ((pwm_t)(PWM_DEFAULT))

typedef enum
{
    MQTR_DIR_pos = ROT_DIR_clock,
    MQTR_DIR_neg = ROT_DIR_anticlock,
} MQTR_DIR_t;

/* clang-format off */
static struct
{   
    pwm_t pwm;
    MQTR_DIR_t   dir;
} mqtr_configs[3] = 
{
    [MQTR_x] = {.pwm = MQTR_PWM_DEFAULT, .dir = MQTR_DIR_pos},
    [MQTR_y] = {.pwm = MQTR_PWM_DEFAULT, .dir = MQTR_DIR_pos},
    [MQTR_z] = {.pwm = MQTR_PWM_DEFAULT, .dir = MQTR_DIR_pos},
};
/* clang-format on */


static pwm_t mqtr_voltage_to_pwm(int voltage_mv);
static int   mqtr_pwm_to_voltage(pwm_t pwm);


void mqtr_set_config(MQTR_t mqtr, int volts_mv)
{
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            mqtr_configs[mqtr].pwm = mqtr_voltage_to_pwm(volts_mv);
            mqtr_configs[mqtr].dir = volts_mv < 0 ? MQTR_DIR_neg : MQTR_DIR_pos;
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
#warning NOT IMPLEMENTED YET
        /** @todo WRITE THE PWM VALUES TO THE CORRECT REGISTERS */
    }
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


int mqtr_config_to_str(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    int voltage_mv_x = mqtr_pwm_to_voltage(mqtr_configs[MQTR_x].pwm);
    int voltage_mv_y = mqtr_pwm_to_voltage(mqtr_configs[MQTR_y].pwm);
    int voltage_mv_z = mqtr_pwm_to_voltage(mqtr_configs[MQTR_z].pwm);
    int required_len = snprintf(
        buf, buflen, "[ %c%d, %c%d, %c%d ]",
        mqtr_configs[MQTR_x].dir == MQTR_DIR_neg ? '-' : '+', voltage_mv_x,
        mqtr_configs[MQTR_y].dir == MQTR_DIR_neg ? '-' : '+', voltage_mv_y,
        mqtr_configs[MQTR_z].dir == MQTR_DIR_neg ? '-' : '+', voltage_mv_z);
    return (required_len < buflen) ? 0 : 1;
}


static pwm_t mqtr_voltage_to_pwm(int voltage_mv)
{
    pwm_t pwm = MQTR_PWM_DEFAULT;
    if (voltage_mv < 0)
    {
        voltage_mv *= -1;
    }

/** @todo IMPLEMENT CONVERSION */
#warning NOT IMPLEMENTED YET

    return pwm;
}


static int mqtr_pwm_to_voltage(pwm_t pwm)
{
    int voltage_mv = 50 * pwm;

/** @todo IMPLEMENT CONVERSION */
#warning NOT IMPLEMENTED YET

    return voltage_mv;
}
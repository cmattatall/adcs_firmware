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
#include "mqtr_timer_pwm_api.h"
#include "targets.h"

static int mqtr_voltage_mv[] = {
    [MQTR_x] = 0,
    [MQTR_y] = 0,
    [MQTR_z] = 0,
};


void mqtr_init(void)
{
    MQTR_PWM_API_init();
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_x, -500);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_y, 1500);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_y, 2500);
}


void mqtr_set_coil_voltage_mv(MQTR_t mqtr, int volts_mv)
{
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            mqtr_voltage_mv[mqtr] = volts_mv;
            MQTR_PWM_API_set_coil_voltage_mv(mqtr, mqtr_voltage_mv[mqtr]);
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
}


int mqtr_config_to_str(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    int required_len =
        snprintf(buf, buflen, "[ %d, %d, %d ]", mqtr_voltage_mv[MQTR_x],
                 mqtr_voltage_mv[MQTR_y], mqtr_voltage_mv[MQTR_z]);
    return (required_len < buflen) ? 0 : 1;
}

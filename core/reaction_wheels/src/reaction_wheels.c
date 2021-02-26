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
#include <assert.h>
#include <string.h>

#if defined(TARGET_MCU)
#include <msp430.h>
#else
#include <stdio.h>
#endif /* #if defined(TARGET_MCU) */

#include "reaction_wheels.h"
#include "targets.h"

/* clang-format off */
static reac_wheel_configs rw_configs = 
{
    .configs = 
    {
        [REAC_WHEEL_x] = 
        {
            .pwm = PWM_DEFAULT,
            .dir = REAC_WHEEL_DIR_pos,
        },
        [REAC_WHEEL_y] = 
        {
            .pwm = PWM_DEFAULT,
            .dir = REAC_WHEEL_DIR_pos,

        },
        [REAC_WHEEL_z] = 
        {
            .pwm = PWM_DEFAULT,
            .dir = REAC_WHEEL_DIR_pos,
        },
    }
};
/* clang-format on */


reac_wheel_configs rw_get_configs(void)
{
    return rw_configs;
}


reac_wheel_config_single rw_get_config(REAC_WHEEL_t wheel)
{
    switch (wheel)
    {
        case REAC_WHEEL_x:
        case REAC_WHEEL_y:
        case REAC_WHEEL_z:
        {
            return rw_configs.configs[wheel];
        }
        break;
        default:
        {
            reac_wheel_config_single error_value = {0};
            return error_value;
        }
        break;
    }
}


void rw_set_config(REAC_WHEEL_t wheel, const reac_wheel_config_single *config)
{
    switch (wheel)
    {
        case REAC_WHEEL_x:
        case REAC_WHEEL_y:
        case REAC_WHEEL_z:
        {
            rw_configs.configs[wheel] = *config;
        }
        break;
        default:
        {
#if !defined(TARGET_MCU)
            printf("API error in %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
        }
        break;
    }
}


void reac_wheel_apply_configs(void)
{
#if defined(TARGET_MCU)
    unsigned int i;
    for (i = 0; i < NUM_REACTION_WHEELS; i++)
    {
        /** @todo WRITE THE PWM VALUES TO THE CORRECT REGISTERS */
    }
#else
    printf("called %s\n", __func__);
#endif /* #if defined(TARGET_MCU) */
}


/** @todo this could be made much cleaner but it works for now */
int reac_wheel_config_to_string(char *buf, unsigned int buflen,
                                const reac_wheel_configs *configs)
{
    CONFIG_ASSERT(NULL != buf);
    int required_len = snprintf(
        buf, buflen, "[ %c%d, %c%d, %c%d ]",
        configs->configs[REAC_WHEEL_x].dir == REAC_WHEEL_DIR_neg ? '-' : '+',
        configs->configs[REAC_WHEEL_x].pwm,
        configs->configs[REAC_WHEEL_y].dir == REAC_WHEEL_DIR_neg ? '-' : '+',
        configs->configs[REAC_WHEEL_y].pwm,
        configs->configs[REAC_WHEEL_z].dir == REAC_WHEEL_DIR_neg ? '-' : '+',
        configs->configs[REAC_WHEEL_z].pwm);
    return required_len < buflen ? 0 : 1;
}

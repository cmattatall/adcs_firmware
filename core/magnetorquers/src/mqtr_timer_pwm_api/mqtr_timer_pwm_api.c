/**
 * @file mqtr_timer_pwm_api.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to separate mqtr timer register level stuff from
 * logic API for magnetorquers
 * @version 0.1
 * @date 2021-03-02
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#if defined(TARGET_MCU)
#include <msp430.h>
#include "mqtr_timer_pwm_api.h"
#else
#endif /* #if defined(TARGET_MCU) */


static void mqtr_x_init_phy(void);
static void mqtr_y_init_phy(void);
static void mqtr_z_init_phy(void);

void mqtr_timer_pwm_init_phy(void)
{
    mqtr_x_init_phy();
    mqtr_y_init_phy();
    mqtr_z_init_phy();
}


#warning NOT IMPLEMENTED YET
#if 0

void mqtr_pwm_set_duty_cycle(MQTR_t mqtr)
{
    switch (mqtr)
    {
        case MQTR_x:
        {
        }
        break;
        case MQTR_y:
        {
        }
        break;
        case MQTR_z:
        {
        }
        break;
    }
}



static void mqtr_x_init_phy(void)
{
    /* Configure F pwm pin */
    P1DIR ^= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */


    /* Configure R pwm pin */
    P1DIR ^= BIT3; /* P1.3 in output direction */
    P1SEL |= BIT3; /* P1.3 will be used for its peripheral function */
}


static void mqtr_y_init_phy(void)
{
    /* Configure F pwm pin */


    /* Configure R pwm pin */
}


static void mqtr_z_init_phy(void)
{
    /* Configure F pwm pin */


    /* Configure R pwm pin */
}


#endif
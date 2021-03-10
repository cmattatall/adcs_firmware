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
#include "timer_a.h"
#include "pwm.h"
#define API_TIMER_OUTMOD_INIT_MODE (OUTMOD_TOG_SET)
#define API_TIMER_MC_INIT_MODE (MC__CONTINUOUS)
#define API_TIMER_TASSEL_INIT_MODE (TASSEL__SMCLK)
#else
#endif /* #if defined(TARGET_MCU) */

#include "magnetorquers.h"
#include "targets.h"

static int mqtr_voltage_mv[] = {
    [MQTR_x] = 0,
    [MQTR_y] = 0,
    [MQTR_z] = 0,
};


static void MQTR_PWM_API_init_phy(void);
static void MQTR_PWM_API_timer_init(void);
static void MQTR_PWM_API_set_x_duty_cycle(float ds_percent);
static void MQTR_PWM_API_set_y_duty_cycle(float ds_percent);
static void MQTR_PWM_API_set_z_duty_cycle(float ds_percent);
static void MQTR_PWM_API_init(void);
static void MQTR_PWM_API_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv);


void MQTR_init(void)
{
    MQTR_PWM_API_init();
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_x, 0);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_y, 0);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_y, 0);
}


void MQTR_set_coil_voltage_mv(MQTR_t mqtr, int volts_mv)
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


int MQTR_config_to_str(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    int required_len =
        snprintf(buf, buflen, "[ %d, %d, %d ]", mqtr_voltage_mv[MQTR_x],
                 mqtr_voltage_mv[MQTR_y], mqtr_voltage_mv[MQTR_z]);
    return (required_len < buflen) ? 0 : 1;
}






/******************************************************************************/
/******************************************************************************
 *
 *
 *       START OF THE TARGET-SPECIFIC REGISTER LEVEL STUFF. IF THIS
 *       SECTION GETS TOO BIG IT SHOULD BE MOVED INTO THE DRIVER LAYER  
 *
 *
 ******************************************************************************/
/******************************************************************************/


static void MQTR_PWM_API_init(void)
{
    MQTR_PWM_API_init_phy();
    MQTR_PWM_API_timer_init();
}


static void MQTR_PWM_API_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv)
{
    if (voltage_mv < -PWM_VMAX_MV_float)
    {
        voltage_mv = -PWM_VMAX_MV_float;
    }
    else if (voltage_mv > PWM_VMAX_MV_float)
    {
        voltage_mv = PWM_VMAX_MV_float;
    }

    float duty_cycle;
    duty_cycle = (voltage_mv * PWM_MAX_DUTY_CYCLE_float) / PWM_VMAX_MV_float;
    switch (mqtr)
    {
        case MQTR_x:
        {
            MQTR_PWM_API_set_x_duty_cycle(duty_cycle);
        }
        break;
        case MQTR_y:
        {
            MQTR_PWM_API_set_y_duty_cycle(duty_cycle);
        }
        break;
        case MQTR_z:
        {
            MQTR_PWM_API_set_z_duty_cycle(duty_cycle);
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
}


static void MQTR_PWM_API_init_phy(void)
{
#if defined(TARGET_MCU)

    /* Configure X coil F pwm pin */
    P1DIR ^= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */

    /* Configure X coil R pwm pin */
    P1DIR ^= BIT3; /* P1.3 in output direction */
    P1SEL |= BIT3; /* P1.3 will be used for its peripheral function */

    /* Configure Y coil R pwm pin */
    P2DIR ^= BIT0; /* P2.0 in output direction */
    P2SEL |= BIT0; /* P2.0 will be used for its peripheral function */

    /* Configure Y coil R pwm pin */
    P2DIR ^= BIT1; /* P2.1 in output direction */
    P2SEL |= BIT1; /* P2.1 will be used for its peripheral function */

    /* Configure Z coil F pwm pin */
    P1DIR ^= BIT4; /* P1.4 in output direction */
    P1SEL |= BIT4; /* P1.4 will be used for its peripheral function */

    /* Configure Z coil R pwm pin */
    P1DIR ^= BIT5; /* P1.5 in output direction */
    P1SEL |= BIT5; /* P1.5 will be used for its peripheral function */

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_timer_init(void)
{
#if defined(TARGET_MCU)

    TA0CTL &= ~(MC0 | MC1); /* Stop timer A0 */

    /* Set Timer A0 clock source to smclk */
    TA0CTL = (TA0CTL & ~(TASSEL0 | TASSEL1)) | API_TIMER_TASSEL_INIT_MODE;

    /* Config for mqtr X face F pin (pin 1.2) */
    TA0CCR1  = 0;
    TA0CCTL1 = (TA0CCTL1 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;

    /* Config for mqtr X face R pin (pin 1.3) */
    TA0CCR2  = 0;
    TA0CCTL2 = (TA0CCTL2 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;


    /* Config for mqtr Z face F pin (pin 1.4) */
    TA0CCR3  = 0;
    TA0CCTL3 = (TA0CCTL3 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;

    /* Config for mqtr Z face R pin (pin 1.5) */
    TA0CCR4  = 0;
    TA0CCTL4 = (TA0CCTL4 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;

    /* Set timer A0 count mode */
    TA0CTL = (TA0CTL & ~(MC0 | MC1)) | API_TIMER_MC_INIT_MODE;


    /* Stop timer A1 */
    TA1CTL &= ~(MC0 | MC1);

    /* Set Timer A1 clock source to smclk */
    TA1CTL = (TA1CTL & ~(TASSEL0 | TASSEL1)) | API_TIMER_TASSEL_INIT_MODE;

    /* Config for mqtr Y face F pin (pin 2.0) */
    TA1CCR1  = 0;
    TA1CCTL1 = (TA1CCTL1 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;

    /* Config for mqtr Y face R pin (pin 2.1)*/
    TA1CCR2  = 0;
    TA1CCTL2 = (TA1CCTL2 & ~(OUTMOD0 | OUTMOD1 | OUTMOD2)) |
               API_TIMER_OUTMOD_INIT_MODE;

    /* Set Timer A1 count mode */
    TA1CTL = (TA1CTL & ~(MC0 | MC1)) | API_TIMER_MC_INIT_MODE;

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_x_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    if (pct_ds == PWM_MIN_DUTY_CYCLE_float)
    {
        TA0CCR1 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
    }
    else if (pct_ds < 0)
    {
        if (pct_ds < -1 * PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = -1 * PWM_MAX_DUTY_CYCLE_float;
        }
        TA0CCR1 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR2 =
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }
        TA0CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR1 =
            (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_y_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    if (pct_ds == PWM_MIN_DUTY_CYCLE_float)
    {
        TA1CCR1 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA1CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
    }
    if (pct_ds < 0.0f)
    {
        if (pct_ds < -1 * PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = -1 * PWM_MAX_DUTY_CYCLE_float;
        }
        TA1CCR1 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA1CCR2 =
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }
        TA1CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA1CCR1 = (uint16_t)((pct_ds / PWM_MAX_DUTY_CYCLE_float) * UINT16_MAX);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_z_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    if (pct_ds == PWM_MIN_DUTY_CYCLE_float)
    {
        TA0CCR3 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR4 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
    }
    if (pct_ds < 0.0f)
    {
        if (pct_ds < -1 * PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = -1 * PWM_MAX_DUTY_CYCLE_float;
        }
        TA0CCR3 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR4 =
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }
        TA0CCR4 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR3 = (uint16_t)((pct_ds / PWM_MAX_DUTY_CYCLE_float) * UINT16_MAX);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);
#endif /* #if defined(TARGET_MCU) */
}

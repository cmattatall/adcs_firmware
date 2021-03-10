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
#include <stdio.h>

#if defined(TARGET_MCU)
#include <msp430.h>
#include "timer_a.h"
#include "pwm.h"
#else
#endif /* #if defined(TARGET_MCU) */

#include "reaction_wheels.h"
#include "targets.h"

#warning RW_RPH_PER_PWM_MV NEEDS TO BE UPDATED TO CORRECT CONVERSION FACTOR
/** @todo THIS NEEDS TO BE UPDATED TO THE CORRECT CONVERSION FACTOR */
#define RW_RPH_PER_PWM_MV (1.0f)


static int rw_speed_rph[] = {
    [REAC_WHEEL_x] = 0,
    [REAC_WHEEL_y] = 0,
    [REAC_WHEEL_z] = 0,
};

static void RW_PWM_API_init(void);
static void RW_PWM_API_init_phy(void);
static void RW_PWM_API_timer_init(void);
static int  RW_rph_to_mv(int rph);
static int  RW_mv_to_rph(int mv);
static void RW_PWM_API_set_duty_cycle(REAC_WHEEL_t rw, float pct_ds);
static void RW_PWM_API_set_x_duty_cycle(float pct_ds);
static void RW_PWM_API_set_y_duty_cycle(float pct_ds);
static void RW_PWM_API_set_z_duty_cycle(float pct_ds);
static int  RW_CURRENT_API_measure_x_current_ma(void);
static int  RW_CURRENT_API_measure_y_current_ma(void);
static int  RW_CURRENT_API_measure_z_current_ma(void);


void RW_init(void)
{
    RW_PWM_API_init();
    RW_PWM_API_set_duty_cycle(REAC_WHEEL_x, 0.0f);
    RW_PWM_API_set_duty_cycle(REAC_WHEEL_y, 0.0f);
    RW_PWM_API_set_duty_cycle(REAC_WHEEL_z, 0.0f);
}


void RW_set_speed_rph(REAC_WHEEL_t rw, int rph)
{
    switch (rw)
    {
        case REAC_WHEEL_x:
        case REAC_WHEEL_y:
        case REAC_WHEEL_z:
        {
            rw_speed_rph[rw] = rph;
            int   voltage_mv = RW_rph_to_mv(rw_speed_rph[rw]);
            float duty_cycle = voltage_mv / (float)PWM_VMAX_MV_float;
            RW_PWM_API_set_duty_cycle(rw, duty_cycle);
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
}


int RW_config_to_string(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    int x_mv    = RW_rph_to_mv(rw_speed_rph[REAC_WHEEL_x]);
    int y_mv    = RW_rph_to_mv(rw_speed_rph[REAC_WHEEL_y]);
    int z_mv    = RW_rph_to_mv(rw_speed_rph[REAC_WHEEL_z]);
    int req_len = snprintf(buf, buflen, "[ %d, %d, %d ]", x_mv, y_mv, z_mv);
    return (req_len < buflen) ? 0 : 1;
}


int RW_measure_current_ma(REAC_WHEEL_t wheel)
{
#if defined(TARGET_MCU)
    switch (wheel)
    {
        case REAC_WHEEL_x:
        {
            return RW_CURRENT_API_measure_x_current_ma();
        }
        break;
        case REAC_WHEEL_y:
        {
            return RW_CURRENT_API_measure_y_current_ma();
        }
        break;
        case REAC_WHEEL_z:
        {
            return RW_CURRENT_API_measure_z_current_ma();
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }

#else

    printf("Called %s with arg %d\n", __func__, wheel);

#endif /* #if defined(TARGET_MCU) */
}


static int RW_rph_to_mv(int rph)
{
    return (int)(rph / ((float)RW_RPH_PER_PWM_MV));
}


static int RW_mv_to_rph(int mv)
{
    return (int)(RW_RPH_PER_PWM_MV * mv);
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


static void RW_PWM_API_init(void)
{
    RW_PWM_API_init_phy();
    RW_PWM_API_timer_init();
}


static void RW_PWM_API_init_phy(void)
{
#if defined(TARGET_MCU)

    /* PIN 2.3 (RW_X_SPEED_CTRL) */
    P2DIR |= BIT3;
    P2SEL |= BIT3;

    /* PIN 2.4 (RW_Y_SPEED_CTRL) */
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    /* PIN 2.5 (RW_Z_SPEED_CTRL) */
    P2DIR |= BIT5;
    P2SEL |= BIT5;

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void RW_PWM_API_timer_init(void)
{
#if defined(TARGET_MCU)

    TA2CTL &= ~(MC0 | MC1); /* Stop timer A2 */

    /* Confgigure pwm on pin 2.3 (TA.2.0) */
    TA2CCR0 = 0;
    TA2CCTL0 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA2CCTL0 |= OUTMOD_TOG_SET;

    /* Confgigure pwm on pin 2.4 (TA.2.1) */
    TA2CCR1 = 0;
    TA2CCTL1 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA2CCTL1 |= OUTMOD_TOG_SET;

    /* Confgigure pwm on pin 2.5 (TA.2.2) */
    TA2CCR2 = 0;
    TA2CCTL2 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA2CCTL2 |= OUTMOD_TOG_SET;


    /* Set Timer A2 clock source to smclk */
    TA2CTL &= ~(TASSEL0 | TASSEL1);
    TA2CTL |= TASSEL__SMCLK;

    /* Set count mode */
    TA2CTL &= ~(MC0 | MC1);
    TA2CTL |= MC__CONTINOUS;
#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void RW_PWM_API_set_duty_cycle(REAC_WHEEL_t rw, float pct_ds)
{
    switch (rw)
    {
        case REAC_WHEEL_x:
        {
            RW_PWM_API_set_x_duty_cycle(pct_ds);
        }
        break;
        case REAC_WHEEL_y:
        {
            RW_PWM_API_set_y_duty_cycle(pct_ds);
        }
        break;
        case REAC_WHEEL_z:
        {
            RW_PWM_API_set_z_duty_cycle(pct_ds);
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
}


static void RW_PWM_API_set_x_duty_cycle(float pct_ds)
{

#if defined(TARGET_MCU)

    if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
    {
        pct_ds = PWM_MAX_DUTY_CYCLE_float;
    }

    TA0CCR0 = (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);

#else

    printf("called %s with arg %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static void RW_PWM_API_set_y_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
    {
        pct_ds = PWM_MAX_DUTY_CYCLE_float;
    }

    TA0CCR1 = (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);

#else

    printf("called %s with arg %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}

static void RW_PWM_API_set_z_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
    {
        pct_ds = PWM_MAX_DUTY_CYCLE_float;
    }

    TA0CCR2 = (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);

#else

    printf("called %s with arg %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static int RW_CURRENT_API_measure_x_current_ma(void)
{
    int current_ma = 50; /* for now, just a stubbed magic number */
#if defined(TARGET_MCU)

#warning NOT IMPLEMENTED YET


#else

    printf("called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
    return current_ma;
}


static int RW_CURRENT_API_measure_y_current_ma(void)
{
    int current_ma = 50; /* for now, just a stubbed magic number */

#if defined(TARGET_MCU)

#warning NOT IMPLEMENTED YET


#else

    printf("called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */

    return current_ma;
}


static int RW_CURRENT_API_measure_z_current_ma(void)
{
    int current_ma = 50; /* for now, just a stubbed magic number */

#if defined(TARGET_MCU)

#warning NOT IMPLEMENTED YET

#else

    printf("called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */

    return current_ma;
}

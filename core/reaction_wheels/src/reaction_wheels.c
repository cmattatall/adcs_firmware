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

#include "targets.h"
#include "reaction_wheels.h"

#if defined(TARGET_MCU)
#include "ads7841e.h"
#include <msp430.h>
#include "timer_a.h"
#include "pwm.h"
#else
#endif /* #if defined(TARGET_MCU) */

#define TIMER_CM_MSK (((CM0) | (CM1)))
#define TIMER_CM_NO_CAPT ((TIMER_CM_MSK) & (CM_0))
#define TIMER_CM_CAP_RISE ((TIMER_CM_MSK) & (CM_1))
#define TIMER_CM_CAP_FALL ((TIMER_CM_MSK) & (CM_2))
#define TIMER_CM_CAP_EDGE ((TIMER_CM_MSK) & (CM_3))

/* Current measurement channels */
/** @todo DON'T FORGET THIS */
#warning FIGURE OUT WHICH CHANNELS CORRESPOND TO WHICH REACTION WHEEL FACES
#warning FIGURE OUT WHICH CHANNELS CORRESPOND TO WHICH REACTION WHEEL FACES
#warning FIGURE OUT WHICH CHANNELS CORRESPOND TO WHICH REACTION WHEEL FACES
#define REAC_WHEEL_ADS7841_CHANNEL_x ADS7841_CHANNEL_SGL_1
#define REAC_WHEEL_ADS7841_CHANNEL_y ADS7841_CHANNEL_SGL_2
#define REAC_WHEEL_ADS7841_CHANNEL_z ADS7841_CHANNEL_SGL_3


/** @todo DON'T FORGET THIS */
#warning THIS CONSTANT NEEDS TO BE UPDATED BASED ON EMPIRICAL MEASUREMENTS OF THE CIRCUITRY
#define REACWHEEL_CURRENT_SENSE_MA_PER_MV (0.5f)


#warning RW_RPH_PER_PWM_MV NEEDS TO BE UPDATED TO CORRECT CONVERSION FACTOR
/** @todo THIS NEEDS TO BE UPDATED TO THE CORRECT CONVERSION FACTOR */
#define RW_RPH_PER_PWM_MV (1.0f)


static int rw_speed_rph[] = {
    [REAC_WHEEL_x] = 0,
    [REAC_WHEEL_y] = 0,
    [REAC_WHEEL_z] = 0,
};
static int RW_rph_to_mv(int rph);

static int RW_current_sense_mv_to_ma(int mv);

static void RW_TIMER_API_init(void);
static void RW_TIMER_API_init_phy(void);
static void RW_TIMER_API_timer_init(void);

static void RW_TIMER_API_set_duty_cycle(REAC_WHEEL_t rw, float pct_ds);

static int RW_TIMER_API_measure_x_current_ma(void);
static int RW_TIMER_API_measure_y_current_ma(void);
static int RW_TIMER_API_measure_z_current_ma(void);

static void RW_ADS7841_CURRENT_MEASUREMENT_PHY_init(void);
static void RW_ADS7841_CURRENT_MEASUREMENT_PHY_deinit(void);


void RW_init(void)
{
    RW_TIMER_API_init();
    RW_TIMER_API_set_duty_cycle(REAC_WHEEL_x, 0.0f);
    RW_TIMER_API_set_duty_cycle(REAC_WHEEL_y, 0.0f);
    RW_TIMER_API_set_duty_cycle(REAC_WHEEL_z, 0.0f);
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
            RW_TIMER_API_set_duty_cycle(rw, duty_cycle);
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
            return RW_TIMER_API_measure_x_current_ma();
        }
        break;
        case REAC_WHEEL_y:
        {
            return RW_TIMER_API_measure_y_current_ma();
        }
        break;
        case REAC_WHEEL_z:
        {
            return RW_TIMER_API_measure_z_current_ma();
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
    return 0;

#endif /* #if defined(TARGET_MCU) */
}


static int RW_rph_to_mv(int rph)
{
    return (int)(rph / ((float)RW_RPH_PER_PWM_MV));
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


static void RW_TIMER_API_init(void)
{
    RW_TIMER_API_init_phy();
    RW_TIMER_API_timer_init();
}


static void RW_TIMER_API_init_phy(void)
{
#if defined(TARGET_MCU)

    /* P3.5 is RW_X_SPEED_CTRL == TB0.5 */
    P3DIR |= BIT5;
    P3SEL |= BIT5;

    /* PIN 2.4 is RW_Y_SPEED_CTRL == TA2.1 */
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    /* PIN 2.5 is RW_Z_SPEED_CTRL == TA2.2*/
    P2DIR |= BIT5;
    P2SEL |= BIT5;

    /* Pin 3.6 is RW_X_OUTFG == TB0.6 */
    P3DIR &= ~BIT6;
    P3DIR |= BIT6;

    /* Pin 5.7 is RW_Y_OUTFG == TB0.1 */
    P5DIR &= ~BIT7;
    P5SEL |= BIT7;

    /* Pin 7.4 is RW_Z_OUTFG == TB0.2 */
    P7DIR &= ~BIT4;
    P7SEL |= BIT4;
#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void RW_TIMER_API_timer_init(void)
{
#if defined(TARGET_MCU)

    /**********************/
    /* Configure timer A2 */
    /**********************/

    TA2CTL &= ~(MC0 | MC1); /* Stop timer A2 */

    /* Confgigure RW_Y pwm on pin 2.4 (TA.2.1) */
    TA2CCR1 = 0;
    TA2CCTL1 &= ~OUTMOD_MSK;
    TA2CCTL1 |= OUTMOD_TOG_SET;

    /* Confgigure RW_Z pwm on pin 2.5 (TA.2.2) */
    TA2CCR2 = 0;
    TA2CCTL2 &= ~OUTMOD_MSK;
    TA2CCTL2 |= OUTMOD_TOG_SET;

    /* Set Timer A2 clock source to smclk */
    TA2CTL &= ~(TASSEL0 | TASSEL1);
    TA2CTL |= TASSEL__SMCLK;

    /* Set timer A2 count mode */
    TA2CTL &= ~(MC0 | MC1);
    TA2CTL |= MC__CONTINOUS;

    /**********************/
    /* Configure timer B0 */
    /**********************/

    TB0CTL &= ~(MC0 | MC1); /* Stop timer B0 */

    /* Source Timer B0 from SMCLK */
    TB0CTL &= ~(TBSSEL0 | TBSSEL1);
    TB0CTL |= TBSSEL__SMCLK;

    /* RW_X pwm on pin 3.5 (TB0.5 == pin 3.5) */
    TB0CCR5 = 0;
    TB0CCTL5 &= ~OUTMOD_MSK;
    TB0CCTL5 |= OUTMOD_TOG_SET;

    /* Configure Y_OUTFG capture on pin 7.4 (TB0.2 == P7.4) */
    TB0CCR1 &= ~TIMER_CM_MSK;
    TB0CCR1 |= TIMER_CM_CAP_EDGE;

    /* Set Timer B0 count mode */
    TB0CTL |= MC__CONTINOUS;

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void RW_TIMER_API_set_duty_cycle(REAC_WHEEL_t rw, float pct_ds)
{
#if defined(TARGET_MCU)

    switch (rw)
    {
        case REAC_WHEEL_x:
        {
            if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
            {
                pct_ds = PWM_MAX_DUTY_CYCLE_float;
            }

            TB0CCR5 =
                (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
        }
        break;
        case REAC_WHEEL_y:
        {
            if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
            {
                pct_ds = PWM_MAX_DUTY_CYCLE_float;
            }

            TA0CCR1 =
                (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
        }
        break;
        case REAC_WHEEL_z:
        {
            if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
            {
                pct_ds = PWM_MAX_DUTY_CYCLE_float;
            }

            TA0CCR2 =
                (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * UINT16_MAX);
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }

#else

    printf("Called %s with arg %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static int RW_TIMER_API_measure_x_current_ma(void)
{
    int current_ma = 0;
#if defined(TARGET_MCU)

    ADS7841_driver_init(RW_ADS7841_CURRENT_MEASUREMENT_PHY_init,
                        RW_ADS7841_CURRENT_MEASUREMENT_PHY_deinit,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);

    uint16_t adc_val = ADS7841_measure_channel(REAC_WHEEL_ADS7841_CHANNEL_x);
    current_ma       = RW_current_sense_mv_to_ma(adc_val);

    ADS7841_driver_deinit();

#endif /* #if defined(TARGET_MCU) */
    return current_ma;
}


static int RW_TIMER_API_measure_y_current_ma(void)
{
    int current_ma = 0;

#if defined(TARGET_MCU)

    ADS7841_driver_init(RW_ADS7841_CURRENT_MEASUREMENT_PHY_init,
                        RW_ADS7841_CURRENT_MEASUREMENT_PHY_deinit,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);

    uint16_t adc_val = ADS7841_measure_channel(REAC_WHEEL_ADS7841_CHANNEL_y);
    current_ma       = RW_current_sense_mv_to_ma(adc_val);

    ADS7841_driver_deinit();

#endif /* #if defined(TARGET_MCU) */

    return current_ma;
}


static int RW_TIMER_API_measure_z_current_ma(void)
{
    int current_ma = 0;

#if defined(TARGET_MCU)

    ADS7841_driver_init(RW_ADS7841_CURRENT_MEASUREMENT_PHY_init,
                        RW_ADS7841_CURRENT_MEASUREMENT_PHY_deinit,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);

    uint16_t adc_val = ADS7841_measure_channel(REAC_WHEEL_ADS7841_CHANNEL_z);
    current_ma       = RW_current_sense_mv_to_ma(adc_val);

    ADS7841_driver_deinit();

#endif /* #if defined(TARGET_MCU) */

    return current_ma;
}


static void RW_ADS7841_CURRENT_MEASUREMENT_PHY_init(void)
{

#if defined(TARGET_MCU)

    /* P5.0 */
    P5DIR |= BIT0;
    P5OUT ^= ~BIT0; /* ADS7841 chip select is active low */

#endif /* #if defined(TARGET_MCU) */
}


static void RW_ADS7841_CURRENT_MEASUREMENT_PHY_deinit(void)
{

#if defined(TARGET_MCU)

    /* P5.0 */
    P5DIR |= BIT0;
    P5DIR |= BIT0;

#endif /* #if defined(TARGET_MCU) */
}


static int RW_current_sense_mv_to_ma(int mv)
{
    return mv * REACWHEEL_CURRENT_SENSE_MA_PER_MV;
}

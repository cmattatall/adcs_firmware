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
#include "ads7841e.h"
#include "timer_a.h"
#include "pwm.h"
#define MQTR_PWM_TIMER_COUNT_MODE MC__UP
#endif /* #if defined(TARGET_MCU) */

#define SMCLK_FREQ (1000000u)

#include "magnetorquers.h"
#include "targets.h"

/** @todo DON'T FORGET THIS */
#warning TODO: THIS NEEDS TO BE UPDATED BASED ON THE HARDWARE CONVERSION FACTOR IN CURRENT SENSING CIRCUITRY
#define MQTR_CURRENT_SENSE_MA_PER_MV (0.5f)

/** @todo DON'T FORGET THIS */
#warning TODO: THESE NEED TO BE UPDATED BASED ON THE HARDWARE MAPPINGS
#define MQTR_CURRENT_SEN_CHANNEL_X ADS7841_CHANNEL_SGL_1
#define MQTR_CURRENT_SEN_CHANNEL_Y ADS7841_CHANNEL_SGL_2
#define MQTR_CURRENT_SEN_CHANNEL_Z ADS7841_CHANNEL_SGL_3

static int mqtr_voltage_mv[] = {
    [MQTR_x] = 0,
    [MQTR_y] = 0,
    [MQTR_z] = 0,
};

static uint16_t MQTR_PWM_API_timer_period_reg_val(uint16_t freq);
static void     MQTR_PWM_API_init_phy(void);
static void     MQTR_PWM_API_timer_init(uint16_t freq);
static void     MQTR_PWM_API_set_x_duty_cycle(float ds_percent);
static void     MQTR_PWM_API_set_y_duty_cycle(float ds_percent);
static void     MQTR_PWM_API_set_z_duty_cycle(float ds_percent);
static void     MQTR_PWM_API_init(void);

static void MQTR_PWM_API_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv);
static void MQTR_current_sense_ads7841_cs_init(void);
static void MQTR_current_sense_ads7841_cs_deinit(void);
static int  MQTR_current_sense_adc_mv_to_ma(int mv);


void MQTR_init(void)
{
    MQTR_PWM_API_init();
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_x, 0);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_y, 0);
    MQTR_PWM_API_set_coil_voltage_mv(MQTR_z, 0);
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


int MQTR_get_coil_voltage_mv(MQTR_t mqtr)
{
    int voltage_mv = 0;
    switch (mqtr)
    {
        case MQTR_x:
        case MQTR_y:
        case MQTR_z:
        {
            voltage_mv = mqtr_voltage_mv[mqtr];
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
    return voltage_mv;
}


int MQTR_config_to_str(char *buf, int buflen)
{
    CONFIG_ASSERT(NULL != buf);
    int required_len =
        snprintf(buf, buflen, "[ %d, %d, %d ]", mqtr_voltage_mv[MQTR_x],
                 mqtr_voltage_mv[MQTR_y], mqtr_voltage_mv[MQTR_z]);
    return (required_len < buflen) ? 0 : 1;
}


int MQTR_get_current_ma(MQTR_t mqtr)
{
    int      current_ma = 0;
    uint16_t adc_val    = 0;

#if defined(TARGET_MCU)
    ADS7841_driver_init(MQTR_current_sense_ads7841_cs_init,
                        MQTR_current_sense_ads7841_cs_deinit,
                        ADS7841_PWRMODE_stayOn, ADS7841_BITRES_12);
    switch (mqtr)
    {
        case MQTR_x:
        {
            adc_val = ADS7841_measure_channel(MQTR_CURRENT_SEN_CHANNEL_X);
        }
        break;
        case MQTR_y:
        {
            adc_val = ADS7841_measure_channel(MQTR_CURRENT_SEN_CHANNEL_Y);
        }
        break;
        case MQTR_z:
        {
            adc_val = ADS7841_measure_channel(MQTR_CURRENT_SEN_CHANNEL_Z);
        }
        break;
        default:
        {
        }
        break;
    }
    current_ma = MQTR_current_sense_adc_mv_to_ma(adc_val);
    ADS7841_driver_deinit();


#else

    printf("Called %s with arg %d. Returning %d\n", __func__, mqtr, current_ma);

#endif /* #if defined(TARGET_MCU) */


    return current_ma;
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
    MQTR_PWM_API_timer_init(1000);
}


static void MQTR_PWM_API_set_coil_voltage_mv(MQTR_t mqtr, int16_t voltage_mv)
{
#if defined(TARGET_MCU)

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

#else

    printf("Called %s with arg %d\n", __func__, voltage_mv);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_init_phy(void)
{
#if defined(TARGET_MCU)

    /* Configure X coil F pwm pin */
    P1DIR |= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */

    /* Configure X coil R pwm pin */
    P1DIR |= BIT3; /* P1.3 in output direction */
    P1SEL |= BIT3; /* P1.3 will be used for its peripheral function */

    /* Configure Y coil R pwm pin */
    P2DIR |= BIT0; /* P2.0 in output direction */
    P2SEL |= BIT0; /* P2.0 will be used for its peripheral function */

    /* Configure Y coil R pwm pin */
    P2DIR |= BIT1; /* P2.1 in output direction */
    P2SEL |= BIT1; /* P2.1 will be used for its peripheral function */

    /* Configure Z coil F pwm pin */
    P1DIR |= BIT4; /* P1.4 in output direction */
    P1SEL |= BIT4; /* P1.4 will be used for its peripheral function */

    /* Configure Z coil R pwm pin */
    P1DIR |= BIT5; /* P1.5 in output direction */
    P1SEL |= BIT5; /* P1.5 will be used for its peripheral function */

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_timer_init(uint16_t pwm_freq)
{
#if defined(TARGET_MCU)

    TA0CTL &= ~(MC0 | MC1); /* Stop timer A0 */

    /* Set Timer A0 clock source to smclk */
    TA0CTL &= ~(TASSEL0 | TASSEL1);
    TA0CTL |= TASSEL__SMCLK;

    TA0CCR0 = MQTR_PWM_API_timer_period_reg_val(pwm_freq);

/* Config for mqtr X face F pin (pin 1.2) */
#warning THIS SEEMS NOT WORK ON THIS SPECIFIC PIN
    TA0CCR1 = 0;
    TA0CCTL1 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA0CCTL1 |= OUTMOD_TOG_SET;
    TA0CCTL1 &= ~(CAP);

    /* Config for mqtr X face R pin (pin 1.3) */
    TA0CCR2 = 0;
    TA0CCTL2 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA0CCTL2 |= OUTMOD_TOG_SET;
    TA0CCTL2 &= ~(CAP);

    /* Config for mqtr Z face F pin (pin 1.4) */
    TA0CCR3 = 0;
    TA0CCTL3 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA0CCTL3 |= OUTMOD_TOG_SET;
    TA0CCTL3 &= ~(CAP);

    /* Config for mqtr Z face R pin (pin 1.5) */
    TA0CCR4 = 0;
    TA0CCTL4 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA0CCTL4 |= OUTMOD_TOG_SET;
    TA0CCTL4 &= ~(CAP);

    /* Set timer A0 count mode */
    TA0CTL &= ~(MC0 | MC1);
    TA0CTL |= MQTR_PWM_TIMER_COUNT_MODE;

    /* Stop timer A1 */
    TA1CTL &= ~(MC0 | MC1);

    TA1CCR0 = MQTR_PWM_API_timer_period_reg_val(pwm_freq);

    /* Set Timer A1 clock source to smclk */
    TA1CTL &= ~(TASSEL0 | TASSEL1);
    TA1CTL |= TASSEL__SMCLK;

    /* Config for mqtr Y face F pin (pin 2.0) */
    TA1CCR1 = 0;
    TA1CCTL1 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA1CCTL1 |= OUTMOD_TOG_SET;

    /* Config for mqtr Y face R pin (pin 2.1)*/
    TA1CCR2 = 0;
    TA1CCTL2 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA1CCTL2 |= OUTMOD_TOG_SET;

    /* Set Timer A1 count mode */
    TA1CTL &= ~(MC0 | MC1);
    TA1CTL |= MQTR_PWM_TIMER_COUNT_MODE;

#else

    printf("Called %s\n", __func__);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_x_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    uint16_t max_count;
    switch (TA0CTL & (MC0 | MC1))
    {
        case MC__CONTINOUS:
        {
            max_count = UINT16_MAX;
        }
        break;
        case MC__UP:
        {
            max_count = TA0CCR0;
        }
        break;
        case MC__UPDOWN:
        {
            max_count = 2 * TA0CCR0;
        }
        break;
        case MC__STOP:
        {
            max_count = 0;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }

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
        TA0CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR1 =
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * max_count);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }

        TA0CCR1 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR2 = (uint16_t)((pct_ds / (PWM_MAX_DUTY_CYCLE_float)) * max_count);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_y_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)

    uint16_t max_count;
    switch (TA1CTL & (MC0 | MC1))
    {
        case MC__CONTINOUS:
        {
            max_count = UINT16_MAX;
        }
        break;
        case MC__UP:
        {
            max_count = TA0CCR0;
        }
        break;
        case MC__UPDOWN:
        {
            max_count = 2 * TA0CCR0;
        }
        break;
        case MC__STOP:
        {
            max_count = 0;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }

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
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * max_count);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }
        TA1CCR2 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA1CCR1 = (uint16_t)((pct_ds / PWM_MAX_DUTY_CYCLE_float) * max_count);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_PWM_API_set_z_duty_cycle(float pct_ds)
{
#if defined(TARGET_MCU)
    uint16_t max_count;
    switch (TA0CTL & (MC0 | MC1))
    {
        case MC__CONTINOUS:
        {
            max_count = UINT16_MAX;
        }
        break;
        case MC__UP:
        {
            max_count = TA0CCR0;
        }
        break;
        case MC__UPDOWN:
        {
            max_count = 2 * TA0CCR0;
        }
        break;
        case MC__STOP:
        {
            max_count = 0;
        }
        break;
        default:
        {
            CONFIG_ASSERT(0);
        }
        break;
    }

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
            (uint16_t)((pct_ds / (-1 * PWM_MAX_DUTY_CYCLE_float)) * max_count);
    }
    else
    {
        if (pct_ds > PWM_MAX_DUTY_CYCLE_float)
        {
            pct_ds = PWM_MAX_DUTY_CYCLE_float;
        }

        TA0CCR4 = (uint16_t)PWM_MIN_DUTY_CYCLE_float;
        TA0CCR3 = (uint16_t)((pct_ds / PWM_MAX_DUTY_CYCLE_float) * max_count);
    }

#else

    printf("called %s with argument %f\n", __func__, pct_ds);
#endif /* #if defined(TARGET_MCU) */
}


static uint16_t MQTR_PWM_API_timer_period_reg_val(uint16_t freq)
{
    uint16_t period_reg_val = 0;
    period_reg_val          = SMCLK_FREQ / freq;
    return period_reg_val;
}


static void MQTR_current_sense_ads7841_cs_init(void)
{
#if defined(TARGET_MCU)

    P5DIR |= BIT1;
    P5OUT &= ~BIT1; /* ADS7841 is active low */

#endif /* #if defined(TARGET_MCU) */
}


static void MQTR_current_sense_ads7841_cs_deinit(void)
{
#if defined(TARGET_MCU)

    P5DIR |= BIT1;
    P5OUT |= BIT1; /* ADS7841 is active low */

#endif /* #if defined(TARGET_MCU) */
}


static int MQTR_current_sense_adc_mv_to_ma(int mv)
{
    return mv * MQTR_CURRENT_SENSE_MA_PER_MV;
}

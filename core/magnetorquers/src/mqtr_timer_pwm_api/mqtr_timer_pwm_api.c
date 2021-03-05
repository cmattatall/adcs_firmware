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
#include "timer_a.h"
#else
#endif /* #if defined(TARGET_MCU) */


#define API_TIMER_OUTMOD_INIT_MODE (OUTMOD_TOG_SET)
#define API_TIMER_MC_INIT_MODE (MC__CONTINUOUS)
#define API_TIMER_TASSEL_INIT_MODE (TASSEL__SMCLK)


static void mqtr_timer_pwm_init_phy(void);
static void mqtr_timer_init(void);


void mqtr_pwm_init(void)
{
    mqtr_timer_pwm_init_phy();
    mqtr_timer_init();
    TA0CCR1 = 20000;
    TA0CCR2 = 20000;
    TA0CCR3 = 20000;
    TA0CCR4 = 20000;
    TA1CCR1 = 20000;
    TA1CCR2 = 20000;
}


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


static void mqtr_timer_pwm_init_phy(void)
{
    /* Configure F pwm pin */
    P1DIR ^= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */

    /* Configure F pwm pin */
    P2DIR ^= BIT0; /* P2.0 in output direction */
    P2SEL |= BIT0; /* P2.0 will be used for its peripheral function */

    /* Configure R pwm pin */
    P2DIR ^= BIT1; /* P2.1 in output direction */
    P2SEL |= BIT1; /* P2.1 will be used for its peripheral function */

    /* Configure R pwm pin */
    P1DIR ^= BIT3; /* P1.3 in output direction */
    P1SEL |= BIT3; /* P1.3 will be used for its peripheral function */

    /* Configure F pwm pin */
    P1DIR ^= BIT4; /* P1.4 in output direction */
    P1SEL |= BIT4; /* P1.4 will be used for its peripheral function */

    /* Configure R pwm pin */
    P1DIR ^= BIT5; /* P1.5 in output direction */
    P1SEL |= BIT5; /* P1.5 will be used for its peripheral function */
}


static void mqtr_timer_init(void)
{
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
}
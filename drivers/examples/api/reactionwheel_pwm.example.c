/**
 * @file reactionwheel_pwm.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Single file executable to demonstrate generating a pwm signal
 * on the reaction wheel pins (x == P3.5, y == P2.4, z == P2.5)
 * using register manipulation
 * @version 0.1
 * @date 2021-03-10
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 *
 * PINOUT:
 * P2.3 == RW_X_SPEED_CTRL
 * P2.4 == RW_Y_SPEED_CTRL
 * P2.5 == RW_Z_SPEED_CTRL
 */

#if defined(TARGET_MCU)
#include <msp430.h>
#include "timer_a.h"
#else
#endif /* #if defined(TARGET_MCU) */

#define API_TIMER_OUTMOD_INIT_MODE (OUTMOD_TOG_SET)
#define API_TIMER_MC_INIT_MODE (MC__CONTINUOUS)
#define API_TIMER_TASSEL_INIT_MODE (TASSEL__SMCLK)


int main()
{
    WDTCTL = WDTPW + WDTHOLD;

    /* P3.5 (RW_X_SPEED_CTRL) */
    P3DIR |= BIT5;
    P3SEL |= BIT5;

    /* PIN 2.4 (RW_Y_SPEED_CTRL) */
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    /* PIN 2.5 (RW_Z_SPEED_CTRL) */
    P2DIR |= BIT5;
    P2SEL |= BIT5;

    TA2CTL &= ~(MC0 | MC1); /* Stop timer A2 */

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

    /* start timerA2 count from 0 to ffff forever */
    TA2CTL |= MC__CONTINOUS;


    TB0CTL &= ~(MC0 | MC1); /* Stop timer B0 */
    TB0CTL &= ~(TBSSEL0 | TBSSEL1);
    TB0CTL |= TBSSEL__SMCLK;

    TB0CCR5 = 0;
    TB0CCTL5 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TB0CCTL5 |= OUTMOD_TOG_SET;
    TB0CTL |= MC__CONTINOUS;

    TA2CCR1 = UINT16_MAX / 2.0f; /* 50% DUTY cycle on P2.4 */
    TA2CCR2 = UINT16_MAX / 2.0f; /* 50% DUTY cycle on P2.5 */
    TB0CCR5 = UINT16_MAX / 2.0f; /* 50% DUTY CYCLE ON PIN 3.5 */

    _BIS_SR(GIE + LPM0_bits);

    while (1)
    {
    }
}
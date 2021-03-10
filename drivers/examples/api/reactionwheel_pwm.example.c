/**
 * @file reactionwheel_pwm.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Single file executable to demonstrate generating a pwm signal
 * on the reaction wheel pins (P2.3, P2.4, P2.5) using register manipulation
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

    /* PIN 2.3 (RW_X_SPEED_CTRL) */
    P2DIR |= BIT3;
    P2SEL |= BIT3;

    /* PIN 2.4 (RW_Y_SPEED_CTRL) */
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    /* PIN 2.5 (RW_Z_SPEED_CTRL) */
    P2DIR |= BIT5;
    P2SEL |= BIT5;

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

    /* so for some reason, uncommenting this line absolultely */
    /* causes timerA2 to stop working */
    // TA2CCR0 = 20000;
    TA2CCR1 = 20000;
    TA2CCR2 = 20000;

    _BIS_SR(GIE + LPM0_bits);

    while (1)
    {
    }
}
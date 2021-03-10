/**
 * @file timerA2_pwm_pin_2_3.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Register level example to produce a pwm waveform using timer A2
 * on pin 2.3
 * @version 0.1
 * @date 2021-03-10
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#include <msp430.h>
#include "timer_a.h"


int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;

    P2DIR |= BIT3; /* output direction */
    P2SEL |= BIT3; /* put  P2.3 into (TA2.0 mode) */

    TA2CTL &= ~(MC0 | MC1); /* Stop timer A0 */

    TA2CCR0 = 0; /* Initialize compare register value to 0 */

    /* Set operation mode upon compare event with TA2CCR0 */
    TA2CCTL0 &= ~(OUTMOD0 | OUTMOD1 | OUTMOD2);
    TA2CCTL0 |= OUTMOD_TOG_SET; /* First compare toggles, second compare sets */

    TA2CTL &= ~(TASSEL0 | TASSEL1);
    TA2CTL |= TASSEL__SMCLK;

    /* Start repeated counts from 0 to ffff */
    TA2CTL &= ~(MC0 | MC1);
    TA2CTL |= MC__CONTINUOUS;

    _BIS_SR(GIE + LPM0_bits);

    TA2CCR0 = 20000; /* Initialize compare register value to 0 */

    while (1)
    {
    }
}
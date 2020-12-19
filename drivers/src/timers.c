/**
 * @file timers.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Driver file for timers on MSP430F5529
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 *
 */
#include <msp430.h>
#include <limits.h>

#include "clocks.h"
#include "timers.h"

volatile bool heartbeat_flag = false;

void TIMERA0_heartbeat_init(void)
{

#if 0
    /* Turn off timer to configure */
    TA0CTL &= ~(MC_0 | MC_1); /* set both count mode bits to 0) */
    TA0CTL |= MC__STOP;
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_2; /* input divide of 2 */
    TA0CTL |= TAIE; /* enable Timer 0 interrupt */

    /* Select SMCLK as base frequency (1.1MHz) */
    TA0CTL &= ~(TASSEL_0 | TASSEL1);
    TA0CTL |= TASSEL__SMCLK;

    /* reach TA0CCR0 once per second. 1.1Mhz ( (1/2)*(1/8)) == 68570 */
    /* but 68570 > 0xffff so best we can do is 0xffff */
    TA0CCR0 = 30000u;

    TA0CCTL0 &= ~(CM0 | CM1); /* disable catpure (set both mode bits to 0) */
    TA0CCTL0 &= ~(CCIS0 | CCIS1);
    TA0CCTL0 |= CCIS_0; /* use CCIxA as TA0CCR0 input signal */
    TA0CCTL0 &= ~(CAP); /* set to compare mode (not capture) */
    TA0CCTL0 |= CCIE;   /* enable capture / compare interrupt */

    /* clear expansion prescaler back to 1 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7; /* set expansion prescaler to 8 */

    TA0CTL |= MC__UP; /* set count mode to count up to TA0CCR0  */
#endif

    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */

    /* 1100000 * (1/8) * (1/8) == 17187.5 */
    /* If we want to blink once per second, we need to interrupt twice per sec*/
    /* (because toggle) */
    /* thus, TA0CCR0 should be 17188/2 == 8593 */
    TA0CCR0 = 8593;
}

/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#else
#error Compiler not supported!
#endif
void TIMERA0_ISR (void)
/* clang-format on */
{

    // P1OUT ^= 0x01;
    static volatile unsigned int x;
    x++;
    if (x > 10)
    {
        P1OUT ^= 0x01;
        x = 0; // Toggle P1.0
    }
}
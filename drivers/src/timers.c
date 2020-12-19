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

#include "timers.h"

volatile bool heartbeat_flag = false;

void TIMERA0_init(void)
{
    TA0CCTL0 = CCIE;            // CCR0 interrupt enabled
    TA0CTL   = TASSEL_2 + MC_1; // SMCLK, contmode
    TA0CCR0  = 1000 - 1;
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

    static volatile unsigned int x;
    x++;
    if (x > 1000)
    {
        P1OUT ^= 0x01;
        x = 0; // Toggle P1.0
    }
}
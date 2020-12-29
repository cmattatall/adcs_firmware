/**
 * @file timer_blinky.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example file to demonstrate usage of timer interrupt driven blinky
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 *
 * @note
 * @todo
 */
#include <msp430.h>
#define delay 1000
static volatile unsigned int x = 0;

int main(void)
{
    WDTCTL   = WDTPW + WDTHOLD; // Stop WDT
    P1DIR    = 0xFF;            // P1.0 output
    TA0CCTL0 = CCIE;            // CCR0 interrupt enabled
    TA0CTL   = TASSEL_2 + MC_1; // SMCLK, contmode
    TA0CCR0  = 1000 - 1;
    _BIS_SR(LPM0_bits + GIE); // Enter LPM0 w/ interrupt
    while (1)
    {
    }
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
void Timer_A(void)
/* clang-format on */
{
    x++;
    if (x > delay)
    {
        P1OUT ^= 0x01;
        x = 0; // Toggle P1.0
    }
}
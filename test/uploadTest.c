/**
 * @file uploadTest.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief The purpose of this file is a simple dual-LED blink example to
 * test a given msp430f5529 launchpad board for programability
 * @version 0.1
 * @date 2020-12-01
 *
 * @copyright Copyright (c) 2020 DSS LORIS project
 */

#include <msp430.h>

int main()
{
    WDTCTL = WDTPW | WDTHOLD; // disable watchdog

    P1DIR |= BIT0 | BIT6; // set P1.0 and P1.6 as output

    P1OUT |= BIT0;  // start with P1.0 on
    P1OUT &= ~BIT6; // and P1.6 off

    while (1) // switch between P1.0 and P1.6 every seconds, until heat death of
              // the universe
    {
        __delay_cycles(1000000); // busy wait 1 sec, what a waste of microwatts!
        P1OUT ^= BIT0 | BIT6;    // switch P1.0 and P1.6
    }
}
/**
 * @file blink.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Very simple single source file to test upload and execution of
 * red LED blink on target device (msp430f5529lp)
 * @version 0.1
 * @date 2020-12-18
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 */

#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    P1DIR |= 0x01;            // Set P1.0 to output direction

    for (;;)
    {
        volatile unsigned int i; // volatile to prevent optimization

        P1OUT ^= 0x01; // Toggle P1.0 using exclusive-OR

        i = 10000; // SW Delay
        do
            i--;
        while (i != 0);
    }

    return 0;
}

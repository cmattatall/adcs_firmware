/*
 * MSP430 Timer Tutorial Example Code 2
 * Anthony Scranney
 * www.Coder-Tronics.com
 * August 2014
 *
 * PWM example using both TimerA_0 and TimerA_1
 * 
 * Taken from https://coder-tronics.com/msp430-timer-pwm-tutorial/ 
 */

#include <msp430.h>

int main(void)
{
    /*** Watchdog timer and clock Set-Up ***/
    WDTCTL  = WDTPW + WDTHOLD; // Stop watchdog timer

    #if 0
    DCOCTL  = 0;               // Select lowest DCOx and MODx
    BCSCTL1 = CALBC1_8MHZ;     // Set range
    DCOCTL  = CALDCO_8MHZ;     // Set DCO step + modulation
    #endif

    /*** GPIO Set-Up ***/
    P1DIR |= BIT2; // P1.2 set as output
    P1SEL |= BIT2; // P1.2 selected Timer0_A Out1
    P2DIR |= BIT1; // P2.1 set as output
    P2SEL |= BIT1; // P2.1 selected Timer1_A Out1
    P2DIR |= BIT4; // P2.4 set as output
    P2SEL |= BIT4; // P2.4 selected Timer1_A Out2

    /*** Timer0_A Set-Up ***/
    TA0CCR0 |= 200 - 1;        // PWM Period
    TA0CCTL1 |= OUTMOD_7;      // TA0CCR1 output mode = reset/set
    TA0CCR1 |= 100;            // TA0CCR1 PWM duty cycle
    TA0CTL |= TASSEL_2 + MC_1; // SMCLK, Up Mode (Counts to TA0CCR0)

    /*** Timer1_A Set-Up ***/
    TA1CCR0 |= 1000 - 1;       // PWM Period
    TA1CCTL1 |= OUTMOD_7;      // TA1CCR1 output mode = reset/set
    TA1CCR1 |= 500;            // TA1CCR1 PWM duty cycle
    TA1CTL |= TASSEL_2 + MC_1; // SMCLK, Up Mode (Counts to TA1CCR0)

    _BIS_SR(LPM0_bits); // Enter Low power mode 0
}
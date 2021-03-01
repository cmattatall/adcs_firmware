/**
 * @file timerA_pwm_copied.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief  This example demonstrates the use of two timers, which are used to
 * output PWM signals on GPIO pins P1.2 and P2. Base on example for msp430g2553 
*  from https://coder-tronics.com/msp430-timer-pwm-tutorial/ 
 * @version 0.1
 * @date 2021-03-01
 * 
 * @copyright Copyright (c) 2021 Carl Mattatall
 */

#include <msp430.h>

int main(void)
{
    /*** Watchdog timer and clock Set-Up ***/
    WDTCTL  = WDTPW + WDTHOLD; // Stop watchdog timer

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
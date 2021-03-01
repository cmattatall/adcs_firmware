/**
 * @file timerA_pwm.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module example to demo generation of pwm signal using timerA
 * @version 0.1
 * @date 2021-03-01
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdlib.h>
#include <msp430.h>
#include <stdint.h>

#define TA0IV_IF1 (0X02)
#define TA0IV_IF2 (0X04)
#define TA0IV_IF3 (0X06)
#define TA0IV_IF4 (0X08)
#define TA0IV_IF5 (0X0A)
#define TA0IV_IF6 (0X0C)
#define TA0IV_OVF (0X0E)

/* clang-format off */
#define OUTMOD_MSK (OUTMOD0 | OUTMOD1 | OUTMOD2)
#define OUTMOD_OUTBIT  (OUTMOD_0)
#define OUTMOD_SET     (OUTMOD_1)
#define OUTMOD_TOG_RST (OUTMOD_2)
#define OUTMOD_SET_RST (OUTMOD_3)
#define OUTMOD_TOGGLE  (OUTMOD_4)
#define OUTMOD_RESET   (OUTMOD_5)
#define OUTMOD_TOG_SET (OUTMOD_6)
#define OUTMOD_RST_SET (OUTMOD_7)
/* clang-format on */


int main(void)
{   
#if 0
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

#endif


    /* Stop WDT */
    WDTCTL = WDTPW + WDTHOLD;

    P1DIR ^= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */

    /* Stop timer A0 */
    TA0CTL &= ~(MC0 | MC1);

    /* Set Timer A0 clock source to smclk */
    TA0CTL &= ~(TASSEL1 | TASSEL1);
    TA0CTL |= TASSEL__SMCLK;

    /* Prescale by 8 */
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID__8;

    /* Set to compare mode (disable capture mode) */
    TA0CCTL0 &= ~CAP;

    /* Compare IRQ will be generated when TA0R == TA0CCR0 */
    TA0CCR0 = 25000;

    TA0CCTL0 &= ~(OUTMOD_MSK);
    TA0CCTL0 |= OUTMOD_SET_RST;

    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */

    /* Enable interrupts */
    TA0CCTL0 |= CCIE;
    TA0CTL |= TAIE;

    /* Start timer in up mode */
    TA0CTL |= MC__CONTINOUS;

    /* Enter LPM0 with interrupts */
    _BIS_SR(GIE);


    while (1)
    {
    }
}

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
#define OUTMOD_MSK     ((OUTMOD0) | (OUTMOD1) | (OUTMOD2))
#define OUTMOD_OUTBIT  ((OUTMOD_0) & (OUTMOD_MSK))
#define OUTMOD_SET     ((OUTMOD_1) & (OUTMOD_MSK))
#define OUTMOD_TOG_RST ((OUTMOD_2) & (OUTMOD_MSK))
#define OUTMOD_SET_RST ((OUTMOD_3) & (OUTMOD_MSK))
#define OUTMOD_TOGGLE  ((OUTMOD_4) & (OUTMOD_MSK))
#define OUTMOD_RESET   ((OUTMOD_5) & (OUTMOD_MSK))
#define OUTMOD_TOG_SET ((OUTMOD_6) & (OUTMOD_MSK))
#define OUTMOD_RST_SET ((OUTMOD_7) & (OUTMOD_MSK))
/* clang-format on */


int main(void)
{
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
    TA0CCTL0 |= OUTMOD_TOGGLE;

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


#if 0
__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    /* Check cause of interrupt */
    switch (TA0IV)
    {
        case TA0IV_IF1: /* TA0R == TA0CCR0 */
        {
        }
        break;
        case TA0IV_IF2: /* TA0R == TA0CCR1 */
        {
        }
        break;
        case TA0IV_IF3: /* TA0R == TA0CCR2 */
        {
        }
        break;
        case TA0IV_IF4: /* TA0R == TA0CCR3 */
        {
        }
        break;
        case TA0IV_IF5: /* TA0R == TA0CCR4 */
        {
        }
        break;
        case TA0IV_IF6: /* TA0R == TA0CCR5 */
        {
        }
        break;
        case TA0IV_OVF: /* Timer overflow */
        {
        }
        break;
        default:
        {
            /* Do nothing */
        }
        break;
    }
}

#endif
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
#if defined(TARGET_MCU)

#include <msp430.h>
#include <stdlib.h>
#include <limits.h>

#include "clocks.h"
#include "timer_a0.h"

#define TA0IV_IF1 (0X02)
#define TA0IV_IF2 (0X04)
#define TA0IV_IF3 (0X06)
#define TA0IV_IF4 (0X08)
#define TA0IV_IF5 (0X0A)
#define TA0IV_IF6 (0X0C)
#define TA0IV_OVF (0X0E)

/* clang-format off */
#define OUTMOD_MSK     (OUTMOD0 | OUTMOD1 | OUTMOD2)
#define OUTMOD_OUTBIT  (OUTMOD_0)
#define OUTMOD_SET     (OUTMOD_1)
#define OUTMOD_TOG_RST (OUTMOD_2)
#define OUTMOD_SET_RST (OUTMOD_3)
#define OUTMOD_TOGGLE  (OUTMOD_4)
#define OUTMOD_RESET   (OUTMOD_5)
#define OUTMOD_TOG_SET (OUTMOD_6)
#define OUTMOD_RST_SET (OUTMOD_7)
/* clang-format on */

volatile bool heartbeat_flag = false;

static callback_handle_t TIMERA0_callback_handle;


void TIMERA0_register_callback(callback_handle_t cb)
{
    TIMERA0_callback_handle = cb;
}

#if 0
    P1DIR ^= BIT2; /* P1.2 in output direction */
    P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */

    TA0CTL &= ~(MC0 | MC1); /* Stop timer */

    TA0CCR1 = TIMERA0_duty_cycle(25);
    TA0CCTL1 |= OUTMOD_TOG_SET;

    /* Set Timer A0 clock source to smclk */
    TA0CTL &= ~(TASSEL1 | TASSEL1);
    TA0CTL |= TASSEL_2;

    /* Set count mode */
    TA0CTL &= ~(MC0 | MC1);
    TA0CTL |= MC__CONTINOUS;

#endif


void TIMERA0_pwm_init_phy(TIMERA0_PWM_PIN_t pwm_pin)
{
    switch (pwm_pin)
    {
        case TIMERA0_PWM_PIN_1:
        {
            P1DIR ^= BIT2; /* P1.2 in output direction */
            P1SEL |= BIT2; /* P1.2 will be used for its peripheral function */
        }
        break;
        case TIMERA0_PWM_PIN_2:
        {
        }
        break;
        case TIMERA0_PWM_PIN_3:
        {
        }
        break;
        case TIMERA0_PWM_PIN_4:
        {
        }
        break;
        case TIMERA0_PWM_PIN_5:
        {
        }
        break;
        case TIMERA0_PWM_PIN_6:
        {
        }
        break;
    }
}


void TIMERA0_heartbeat_init(void)
{
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7; /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;    /* CCR0 interrupt enabled */

    /* source from SMCLK, count up to TA0CCR0 */
    TA0CTL &= ~(TASSEL0 | TASSEL1);
    TA0CTL |= TASSEL__SMCLK;
    TA0CTL |= MC__UP;

    /* 1100000 * (1/8) * (1/8) == 17187.5 */
    /* If we want to blink once per second, we need to interrupt twice per sec*/
    /* (because toggle) */
    /* thus, TA0CCR0 should be 17188/2 == 8593 if we want to blink once per
     * sec*/
    /*
     * BUT, FOR SOME REASON, WHEN I DO THIS, THE DAMN THING BLINKS LIKE 13
     * TIMES A SECOND....
     *
     * SO EITHER THE DATASHEET DOCUMENTATION ON FREQUENCY OF SMCLK IS WRONG, OR
     * SOMETHING ELSE IS GOING ON...
     *
     * TIMERA IS IN SECTION 17.2 OF USER MANUAL
     * https://www.ti.com/lit/ug/slau208q/slau208q.pdf?ts=1608390374754&ref_url=https%253A%252F%252Fwww.google.com%252F
     *
     * for now, just setting the compare value to 50k so we get a heartbeat
     * blink. The entire purpose of the heartbeat blink is to have a visual
     * indication of lockup (since if a chip hardfaults the debugger would
     * also become nonresponsive)
     */
    TA0CCR0 = 50000;
}


__interrupt_vec(TIMER0_A0_VECTOR) void TIMERA0_ISR(void)
{
    switch (TA0IV)
    {
        case TA0IV_IF1: /* TA0R == TA0CCR1 */
        {
        }
        break;
        case TA0IV_IF2: /* TA0R ==  TA0CCR2 */
        {
        }
        break;
        case TA0IV_IF3: /* TA0R == TA0CCR3 */
        {
        }
        break;
        case TA0IV_IF4: /* TA0R == TA0CCR4 */
        {
        }
        break;
        case TA0IV_IF5: /* TA0R == TA0CCR5 */
        {
        }
        break;
        case TA0IV_IF6: /* TA0R == TA0CCR6 */
        {
        }
        break;
        case TA0IV_OVF: /* TA0R == TIMER COUNT OVERFLOW*/
        {
        }
        break;
    }
    // callback_exec(TIMERA0_callback_handle);
}

#else

#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS

#endif /* !defined(TARGET_MCU) */

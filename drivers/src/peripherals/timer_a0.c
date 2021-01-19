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

volatile bool heartbeat_flag = false;

static callback_handle_t TIMERA0_callback_handle;


void TIMERA0_register_callback(callback_handle_t cb)
{
    TIMERA0_callback_handle = cb;
}

void TIMERA0_heartbeat_init(void)
{
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */

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
    callback_exec(TIMERA0_callback_handle);
}

#else

#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS

#endif /* !defined(TARGET_MCU) */




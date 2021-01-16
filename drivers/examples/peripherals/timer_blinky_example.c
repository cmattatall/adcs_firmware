/**
 * @file timer_blinky.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example file to demonstrate usage of timer interrupt driven blinky
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 *
 */
#include <msp430.h>
static volatile int timer_expired = 0;

static void stop_watchdog(void);
static void red_led_init(void);
static void enable_interrupts(void);
static void TIMERA0_init(void);

int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    enable_interrupts();
    while (1)
    {
        if (timer_expired)
        {
            P1OUT ^= 0x01;
            timer_expired = 0;
        }
    }
}


__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    timer_expired = 1;
}


static void stop_watchdog(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
}


static void red_led_init(void)
{
    P1DIR |= 0x01; // Set P1.0 to output direction
}

static void enable_interrupts(void)
{
    _BIS_SR(GIE); // Enter LPM0 w/ interrupt
}


static void TIMERA0_init(void)
{
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */

    /* 1000000 * (1/8) * (1/8) == 15625 */
    /* If we want to blink once per second, we need to interrupt twice per sec*/
    /* (because toggle) */
    /* thus, TA0CCR0 should be 15625/2 == 7812 if we want to blink once per
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
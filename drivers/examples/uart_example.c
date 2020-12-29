/**
 * @file timer_blinky.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example file to demonstrate usart of uart through USCI_A0
 * @version 0.1
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 DSS Loris project
 *
 * @note
 * @todo
 */
#include <stdlib.h>
#include <msp430.h>
#include <stdint.h>

static volatile int rx_cplt_flag;


static uint8_t *    tx_buf; /* buffer index of currently transmitting buffer */
static volatile int tx_idx; /* buffer index of next byte to load into uart */
static volatile int tx_cnt;
static volatile int timer_expired_flag;


static void stop_watchdog(void);
static void red_led_init(void);
static void enable_interrupts(void);
static void TIMERA0_init(void);
static void uart_init(void);
static void uart_transmit_async(uint8_t *msg, uint_least16_t msglen);

static char         rx_buf[250];
static unsigned int rx_buf_idx = 0;

int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    uart_init();
    enable_interrupts();

    while (1)
    {
        if (timer_expired_flag)
        {
            P1OUT ^= 0x01;
            timer_expired_flag = 0;

            uint8_t msg[] = "hi\r\n";
            uart_transmit_async(msg, sizeof(msg));
        }

        if (rx_cplt_flag)
        {
            rx_buf[rx_buf_idx] = UCA0RXBUF;
            rx_buf_idx   = (rx_buf_idx > sizeof(rx_buf)) ? 0 : (rx_buf_idx + 1);
            rx_cplt_flag = 0;
        }
    }
}


__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    timer_expired_flag = 1;
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
    TA0CCR0 = 65000;
}

static void uart_init(void)
{
    P3SEL = BIT3 + BIT4;  /* P3.4,5 = USCI_A0 TXD/RXD */
    UCA0CTL1 |= UCSWRST;  /* **Put state machine in reset** */
    UCA0CTL1 |= UCSSEL_2; /* SMCLK */

    UCA0BR0 = 6; /* 1MHz 9600 (see User's Guide) */
    UCA0BR1 = 0; /* 1MHz 9600 */

    /** @note
     * There is something wrong with the modulation control settings
     * I can transmit SOME stuff but it never goes through coherently...
     */
    UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16; /* Modln UCBRSx=0, UCBRFx=0, */
                                            /* over sampling */
    UCA0CTL1 &= ~UCSWRST;                   /* Initialize USCI state machine */

    UCA0IE |= UCRXIE; /* Enable USCI_A0 RX interrupt */
}


__attribute__((used, interrupt(USCI_A0_VECTOR))) void USCI_A0_VECTOR_ISR(void)
{
    switch (UCA0IV)
    {
        case 0: /* no interrupt */
        {
        }
        break;
        case 2: /* RX interrupt  */
        {
            rx_cplt_flag = 1;
        }
        break;
        case 4: /* TX interrupt */
        {
            /* Load next byte and check for completion */
            tx_idx++;
            UCA0TXBUF = tx_buf[tx_idx];
            if (tx_idx == tx_cnt)
            {
                UCA0IE &= ~UCTXIE; /* Disable Transmit complete interrupt */
            }
        }
        break;
        default: /* not sure what happened here */
        {
        }
        break;
    }
}


static void uart_transmit_async(uint8_t *msg, uint_least16_t msglen)
{
    if (!(UCA0IE & UCTXIE)) /* If No currently ongoing transmit */
    {
        tx_buf    = msg;    /* set tx buffer to start of msg buffer */
        tx_cnt    = msglen; /* set remaining count to message length */
        tx_idx    = 0;
        UCA0TXBUF = tx_buf[tx_idx]; /* load first byte into transmit buffer */
        UCA0IE |= UCTXIE;           /* Enable USCI_A0 TX complete interrupt */
    }
}

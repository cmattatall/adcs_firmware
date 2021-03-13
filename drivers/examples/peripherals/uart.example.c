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
 * The example periodically transmits "Hello World\r\n" through the tx line
 * at 9600 baud with a 1.8% bitrate error.
 *
 * When '!' is received, the message "message received!\r\n" is then
 * echoed back
 */
#include <stdlib.h>
#include <msp430.h>
#include <stdint.h>

#define MSG_DELIM '!'

static void stop_watchdog(void);
static void red_led_init(void);
static void enable_interrupts(void);
static void TIMERA0_init(void);
static void uart_init(void);

static void uart_receive_byte(void);
static void uart_transmit(uint8_t *buf, uint16_t buflen);


static volatile int timer_flag;
static volatile int msg_received;

static uint8_t *             tx_buf;
static volatile unsigned int tx_idx;
static unsigned int          tx_max;


static char         rx_buf[250];
static unsigned int rx_buf_idx;

static void (*uart_rx_cb)(void);




int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    uart_init();
    enable_interrupts();

    uart_rx_cb = uart_receive_byte;

    while (1)
    {
        if (timer_flag)
        {
            P1OUT ^= 0x01;
            timer_flag    = 0;
            uint8_t msg[] = "Hello World";
            uart_transmit(msg, sizeof(msg));
        }

        if (msg_received)
        {
            /* Do stuff with message */

            uint8_t msg[] = "message recevied!\r\n";
            //uart_transmit_start(msg, sizeof(msg));

            msg_received = 0;
        }
    }
}


__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    /* Timer compare register is already maxed at ~65000 */
    /* And I don't want to mess with the prescalers */
    static volatile int x;
    if (++x > 3)
    {
        x          = 0;
        timer_flag = 1;
    }
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
    TA0CCR0  = 6000;
}

static void uart_init(void)
{
    P3SEL = BIT3 + BIT4; /* P3.4,5 = USCI_A0 TXD/RXD */
    UCA0CTL1 |= UCSWRST; /* **Put state machine in reset** */

    UCA0CTL1 |= UCSSEL__SMCLK; /* SMCLK */
    
    UCA0BR0 = 104; /* 9600 baud 8N1 See table 36-5 */ 
    UCA0BR1 = 0;

    UCA0MCTL &= ~UCOS16;
    UCA0MCTL &= ~(UCBRS0 | UCBRS1 | UCBRS2);
    UCA0MCTL |= UCBRS_1;
    UCA0MCTL &= ~(UCBRF0 | UCBRF1 | UCBRF2 | UCBRF3);
    UCA0MCTL |= UCBRF0;

    /* Modln UCBRSx=0, UCBRFx=0, */
    /* over sampling */
    UCA0CTL1 &= ~UCSWRST; /* Initialize USCI state machine */

    UCA0IE |= UCRXIE; /* Enable USCI_A0 RX interrupt */
}


__attribute__((used, interrupt(USCI_A0_VECTOR))) void USCI_A0_VECTOR_ISR(void)
{
    /* See table 39-19 */
    switch (UCA0IV)
    {
        case 0x02: /* Receive buffer full */
        {
            uart_rx_cb();
        }
        break;
        case 0x06: /* Start bit received */
        {
        }
        break;
        case 0x08: /* Transmit complete */
        {
            /*
             * This occurs when last bit from hardware shift register
             * is sent DOWN the line. Typically we would use this
             * to relinquish control of the line in a half-duplex
             * scenario. For UART it's not that useful because the bus
             * is full duplex
             */
        }
        break;
        default: /* not sure what happened here */
        {
        }
        break;
    }
}

static void uart_receive_byte(void)
{
    rx_buf[rx_buf_idx] = UCA0RXBUF;
    if (rx_buf[rx_buf_idx] == MSG_DELIM)
    {
        msg_received = 1;
    }
    rx_buf_idx = (rx_buf_idx > sizeof(rx_buf)) ? 0 : (rx_buf_idx + 1);
}



static void uart_transmit(uint8_t *buf, uint16_t buflen)
{
    unsigned int i = 0;
    do
    {
        if(UCA0IFG & UCTXIFG)
        {   
            while(!(UCA0STAT & UCBUSY))
            {
                UCA0TXBUF = buf[i++];
            }
        }
    }
    while(i < buflen);
}
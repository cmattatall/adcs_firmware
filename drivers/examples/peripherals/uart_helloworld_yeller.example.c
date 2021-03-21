/**
 * @file uart_helloworld_yeller.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to have msp430f5529 yell repeatedly "Hello World"
 * @version 0.1
 * @date 2021-03-21
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void uart_init(void);
int  uart_transmit(uint8_t *msg, uint_least16_t msglen);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; /* Stop watchdog timer */
    uart_init();
    while (1)
    {
        volatile uint16_t i = 0;
        while (++i < 30000)
            ;

        uint8_t msg[] = "Hello World";
        uart_transmit(msg, sizeof(msg));
    }
}


void uart_init(void)
{
    P3SEL |= BIT3; /* P3.3 = UCA0TX */
    P3SEL |= BIT4; /* P3.4 = UCA0RX */

    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL__SMCLK; /* SMCLK */

    UCA0BR0 = 104; /* 9600 baud 8N1 See table 36-5 */
    UCA0BR1 = 0;

    UCA0MCTL &= ~UCOS16;
    UCA0MCTL &= ~(UCBRS0 | UCBRS1 | UCBRS2);
    UCA0MCTL |= UCBRS_1;
    UCA0MCTL &= ~(UCBRF0 | UCBRF1 | UCBRF2 | UCBRF3);
    UCA0MCTL |= UCBRF0;

    UCA0CTL1 &= ~UCSWRST; /* Initialize USCI state machine */

    UCA0IE |= UCRXIE; /* Enable USCI_A0 RX interrupt */
}


int uart_transmit(uint8_t *msg, uint_least16_t msglen)
{
    if (!(UCA0IE & UCTXIE) && !(UCA0STAT & UCBUSY))
    {
        unsigned int i = 0;
        do
        {
            if (UCA0IFG & UCTXIFG)
            {
                UCA0TXBUF = msg[i++];
            }
        } while (i < msglen);
    }
    return 0;
}
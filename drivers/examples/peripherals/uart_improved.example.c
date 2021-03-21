/**
 * @file uart_improved.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to provide example program for an improved UART
 *
 * @version 0.1
 * @date 2021-03-21
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 *
 * PINOUT:
 * P3.3 = MCU TX
 * P3.4 = MCU RX
 */

// drivers/examples/ADCS_DRIVERS_uart_improved.example.elf

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void uart_init(void);
int  uart_transmit(uint8_t *msg, uint_least16_t msglen);
int  uart_receive_bytes(uint8_t *caller_buf, uint16_t caller_buflen);

#define UART_BUFLEN 200u

#define UART_RX_MSG_DELIM '!'

static volatile uint8_t  uart_rxbuf[UART_BUFLEN];
static volatile uint8_t *uart_rx_inptr;
static volatile uint8_t *uart_rx_outptr;

static volatile int uart_rx_delim_received;

static uint8_t uart_txbuf[UART_BUFLEN];
#define uart_printf(fmt, ...)                                                  \
    do                                                                         \
    {                                                                          \
        uint16_t cnt;                                                          \
        cnt = snprintf(uart_txbuf, sizeof(uart_txbuf), (fmt), ##__VA_ARGS__);  \
        if (cnt > sizeof(uart_txbuf))                                          \
        {                                                                      \
            cnt = sizeof(uart_txbuf);                                          \
        }                                                                      \
        uart_transmit(uart_txbuf, cnt);                                        \
    } while (0)

uint8_t caller_rxbuf[200];


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; /* Stop watchdog timer */
    uart_init();
    __bis_SR_register(GIE);

    while (1)
    {
        if (uart_rx_delim_received)
        {
            uart_receive_bytes(caller_rxbuf, sizeof(caller_rxbuf));
            uart_printf("received: %s", caller_rxbuf);
            uart_rx_delim_received = 0;
        }
    }
}


void uart_init(void)
{
    uart_rx_inptr  = uart_rxbuf;
    uart_rx_outptr = uart_rxbuf;

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


__interrupt_vec(USCI_A0_VECTOR) void USCI_A0_ISR(void)
{

    /* See table 39-19 */
    switch (UCA0IV)
    {
        case 0x02: /* Receive buffer full */
        {
            *uart_rx_inptr = UCA0RXBUF;

            if (*uart_rx_inptr == UART_RX_MSG_DELIM)
            {
                *uart_rx_inptr         = '\0';
                uart_rx_delim_received = 1;
            }

            if (++uart_rx_inptr > uart_rxbuf + sizeof(uart_rxbuf))
            {
                uart_rx_inptr = uart_rxbuf;
            }
        }
        break;
        case 0x06: /* Start bit received */
        {
        }
        break;
        case 0x08: /* Transmit complete (last bit was sent to PHY)
                    */
        {
        }
        break;
        default: /* not sure what happened here */
        {
        }
        break;
    }
}


int uart_receive_bytes(uint8_t *caller_buf, uint16_t caller_buflen)
{
    int retval      = 0;
    int i           = 0;
    int delim_found = 0;
    do
    {
        caller_buf[i] = *uart_rx_outptr;
        if (caller_buf[i] == '\0')
        {
            delim_found = 1;
        }

        uart_rx_outptr++;
        if (uart_rx_outptr > uart_rxbuf + sizeof(uart_rxbuf))
        {
            uart_rx_outptr = uart_rxbuf;
        }
    } while (++i < caller_buflen && !delim_found);

    if (!delim_found)
    {
        retval = -1;
    }
    else
    {
        retval = i;
    }

    return retval;
}

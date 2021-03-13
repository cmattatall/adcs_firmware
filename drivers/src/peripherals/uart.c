/**
 * @file uart.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief UART peripheral file
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* memcpy */

#include "attributes.h"

#include "targets.h"

#include <msp430.h>

#include "uart.h"

static receive_func uart_rx_cb;

static uint8_t *             tx_buf;
static volatile unsigned int tx_idx;
static unsigned int          tx_max;

void uart_init(receive_func rx)
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

    /* over sampling */
    UCA0CTL1 &= ~UCSWRST; /* Initialize USCI state machine */

    log_trace("initialized uart\n");
    CONFIG_ASSERT(rx != NULL);
    uart_rx_cb = rx;

    UCA0IE |= UCRXIE; /* Enable USCI_A0 RX interrupt */
}

void uart_deinit(void)
{
    /** @todo
     *  - RESET REGISTERS TO DEFAULT VAULES
     *  - RESET CONFIGURATION OF USCI_A0 to defaults
     *  - DISABLE CLOCK MUX to peripheral (optional - improves power savings)
     */

    if (uart_rx_cb != NULL)
    {
        uart_rx_cb = NULL;
    }

    /* Disable interrupts */
    UCA0IE = 0;

    /* Clear any pending interrupt flags */
    UCA0IFG = 0;

    log_trace("deinitialized uart\n");
}


int uart_transmit(uint8_t *msg, uint_least16_t msglen)
{
    CONFIG_ASSERT(msg != NULL);
    if (!(UCA0IE & UCTXIE) && !(UCA0STAT & UCBUSY))
    {   
        unsigned int i = 0;
        do
        {
            if(UCA0IFG & UCTXIFG)
            {
                UCA0TXBUF = msg[i++];
            }
        }
        while(i < msglen);
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
            uart_rx_cb(UCA0RXBUF);
        }
        break;
        case 0x06: /* Start bit received */
        {
        }
        break;
        case 0x08: /* Transmit complete (last bit was sent to PHY) */
        {
        }
        break;
        default: /* not sure what happened here */
        {
        }
        break;
    }
}
/**
 * @file uart.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief UART peripheral file
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 * @note
 * @todo
 */
#include <msp430.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* memcpy */
#include "uart.h"

void uart_init(void)
{
    /** @todo initialize the UART peripheral with
     *  - CONFIGURE RCC CLOCK MUX TO peripheral (very first thing)
     *  -
     *  - baudrate
     *  - parity
     *  - # stopbits
     *
     *  - configure interrupt mask bits
     *  - unmask interrupt
     */
}

void uart_deinit(void)
{
    /** @todo
     *  - RESET REGISTERS TO DEFAULT VAULES
     *  - FLUSH RX AND TF FIFO
     *  - CLEAR INTERRUPT FLAGS
     *  - DISABLE INTERRUPTS
     *  - RESET CONFIGURATION OF USCI_A0 to defaults
     *  - DISABLE CLOCK MUX to peripheral (optional - improves power savings)
     */
}


int uart_transmit(uint8_t *buf, uint_least16_t buflen)
{
#if defined(DEBUG)
    if (buf == NULL)
    {
        while (1)
        {
            /* hang */
        }
    }
#endif /* #if defined(DEBUG) */

    /** @todo IMPLEMENT */
}


int uart_receive(uint8_t *buf, uint_least16_t buflen)
{
#if defined(DEBUG)
    if (buf == NULL)
    {
        while (1)
        {
            /* hang */
        }
    }
#endif /* #if defined(DEBUG) */

    /** @todo IMPLEMENT */
}


__attribute__((interrupt(USCI_A0_VECTOR))) void USCI_A0_ISR(void)
{
    /** @todo IMPLEMENT */
}

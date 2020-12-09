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
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* memcpy */
#include "uart.h"

#include "utils.h"


static volatile bool tx_cplt = false;


static void (*uart_receive_byte)(uint8_t);

/** @note PLEASE READ https://www.ti.com/lit/ug/slau144j/slau144j.pdf
 * PAGE 424 before touching this function
 */
void uart_init(void (*receive_byte_func)(uint8_t))
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop WDT
    P3SEL  = BIT3 + BIT4;                   // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                    // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0BR0  = 6;                           // 1MHz 9600 (see User's Guide)
    UCA0BR1  = 0;                           // 1MHz 9600
    UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16; // Modln UCBRSx=0, UCBRFx=0,
                                            // over sampling
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

    CONFIG_ASSERT(receive_byte_func != NULL);
    uart_receive_byte = receive_byte_func;
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

    if (uart_receive_byte != NULL)
    {
        uart_receive_byte = NULL;
    }
}


int uart_transmit(uint8_t *buf, uint_least16_t buflen)
{
    CONFIG_ASSERT(buf != NULL);
    tx_cplt         = true;
    uint_fast16_t i = 0;
    do
    {
        if (tx_cplt)
        {
            i++;
            tx_cplt = false;
        }
        UCA0TXBUF = buf[i]; /* load into transmit buffer after */
    } while (i <= buflen);
}


/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
#else
#error Compiler not supported!
#endif
void USCI_A0_ISR(void)
/* clang-format on */
{
    switch (UCA0IV)
    {
        case 0: /* no interrupt */
        {
        }
        break;
        case 2: /* RX interrupt  */
        {
            uart_receive_byte(UCA0RXBUF);
        }
        break;
        case 4: /* TX interrupt */
        {
            CONFIG_ASSERT(tx_cplt != true);
            tx_cplt = true;
        }
        break;
        default: /* not sure what happened here */
        {
            CONFIG_ASSERT(0);
        }
        break;
    }
}

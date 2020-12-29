/**
 * @file uart_example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to demonstrate use of the UART through USCI_A0
 * @version 0.1
 * @date 2020-12-29
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

static int  uart_transmit(uint8_t *buf, uint_least16_t buflen);
static void uart_init(void);

static volatile bool tx_cplt = false;
static volatile bool rx_cplt = false;


int main(void)
{
    uart_init();

    char msg[] = "Hello World!\r\n";
    while (1)
    {
        /* If transmitter is busy */
        if (!tx_cplt)
        {
            uart_transmit((uint8_t*)msg, sizeof(msg));
        }

        if (rx_cplt)
        {
        }
    }
}


static void uart_init(void)
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
}


static int uart_transmit(uint8_t *buf, uint_least16_t buflen)
{
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
    tx_cplt = false;
    return i;
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
            rx_cplt = true;
        }
        break;
        case 4: /* TX interrupt */
        {
            tx_cplt = true;
        }
        break;
        default: /* not sure what happened here */
        {
        }
        break;
    }
}

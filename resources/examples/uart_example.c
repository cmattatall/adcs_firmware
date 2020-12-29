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

static int uart_transmit(uint8_t *buf, uint_least16_t buflen);

static volatile bool tx_cplt = false;


int main(void)
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
    log_trace("initialized uart\n");
    CONFIG_ASSERT(rx != NULL);
    uart_receive_byte = rx;
}



static int uart_transmit(uint8_t *buf, uint_least16_t buflen)
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

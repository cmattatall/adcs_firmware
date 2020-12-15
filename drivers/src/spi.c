/**
 * @file spi.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-05
 *
 * @copyright Copyright (c) 2020 LORRIS ADCS
 *
 * @note
 * @todo
 */

/** @todo THIS SECTION REALLLLLY NEEDS TO BE WRAPPED INTO A SEPARATE
 * THING compiler.h header or something */


#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* memcpy */

#include "targets.h"

#if defined(TARGET_MCU)

#include <msp430.h>
#include "spi.h"

static uint8_t *         rx_outptr;
static volatile uint8_t *rx_inptr;
static uint8_t           rx_ringbuf[SPI_APPLICATION_BUFFER_SIZE];
static volatile int      rx_complete_signal = SPI_SIGNAL_CLR;


static uint8_t               tx_fifo[SPI_APPLICATION_BUFFER_SIZE];
static volatile unsigned int tx_count;
static unsigned int          tx_bytes_loaded;
static volatile int          tx_complete_signal = SPI_SIGNAL_CLR;


void SPI0_init(volatile int **receive_signal_watcher,
               volatile int **transmit_signal_watcher)
{
    /* Configure receive interface */
    rx_inptr           = rx_ringbuf;
    rx_outptr          = rx_ringbuf;
    tx_complete_signal = SPI_SIGNAL_CLR;
    if (receive_signal_watcher != NULL)
    {
        *receive_signal_watcher = &rx_complete_signal;
    }


    /* Configure transmit interface */
    tx_complete_signal = SPI_SIGNAL_SET;
    tx_count           = 0;
    tx_bytes_loaded    = 0;
    if (transmit_signal_watcher != NULL)
    {
        *transmit_signal_watcher = &tx_complete_signal;
    }

    UCB0CTL1 |= UCSWRST; /* unlock ie: "reset" peripheral */

    /* Configure control registers */
    UCB0CTL0 |= UCMST;    /* master */
    UCB0CTL0 |= UCMODE_0; /* mode 0 */
    UCB0CTL0 |= UCMSB;    /* receive via msb first */
    UCB0CTL0 |= UCSYNC;   /* Select SPI mode */
    UCB0CTL0 |= UCCKPH;   /* falling edge clock phase (default) */
    UCB0CTL1 |= UCSSEL_2;

    /* Configure bitrate registers */
    UCB0BR1 = 0x00;
    UCB0BR0 = 0x01;

    /* Re-enable peripheral */
    UCB0CTL1 &= ~UCSWRST; /* Lock the peripheral control register config */

#if defined(SPI0_LOOPBACK)
#warning SPI0 is configured in LOOPBACK mode
    UCB0STAT |= UCLISTEN;
#else
    UCB0STAT &= ~UCLISTEN;
#endif /* #if defined(SPI0_LOOPBACK) */

    /* Enable receive complete interrupt */
    UCB0IE |= UCRXIE;

    /* Configure GPIO ports */
    P3SEL = BIT0 | BIT1 | BIT2; // Configures SPI
    P2OUT &= ~BIT2;             // select device
    P3OUT |= BIT1;
    P2OUT |= BIT2;
    P2DIR |= BIT2;        // CONFIGURE CS FOR OUTPUT
    P3DIR |= BIT0 | BIT2; // Set as outputs
    P3DIR &= ~BIT1;

    P2OUT &= ~BIT2; // Make CS Low
    log_trace("SPI 0 initialized!\n");
}

int SPI0_receive_payload(uint8_t *userbuf, uint16_t len)
{
    uint_fast16_t i           = 0;
    uint_fast16_t delim_found = 0;
    do
    {
        userbuf[i] = *rx_outptr;
        rx_outptr++;
        if (rx_outptr == &rx_ringbuf[sizeof(rx_ringbuf) - 1])
        {
            rx_outptr = rx_ringbuf;
        }

        if (userbuf[i] == SPI_DELIM_CHAR)
        {
            delim_found = 1;
            if (rx_complete_signal == SPI_SIGNAL_SET)
            {
                rx_complete_signal = SPI_SIGNAL_CLR;
            }
        }

    } while (++i < len && !delim_found);

    if (delim_found)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


unsigned int SPI0_transmit_IT(uint8_t *bytes, uint16_t len)
{
    /* Don't clobber ongoing transmission */
    if (tx_complete_signal == SPI_SIGNAL_CLR)
    {
        return 0; /* failure */
    }
    else
    {
        /* Clear flag var */
        tx_complete_signal = SPI_SIGNAL_CLR;

        uint_fast16_t copy_len = len;

        /* Only load as much as we can fit */
        if (len > sizeof(tx_fifo))
        {
            copy_len = sizeof(tx_fifo);
        }

        memcpy(tx_fifo, bytes, copy_len); /** @todo TRANSFER VIA DMA */
        tx_count        = 0;
        tx_bytes_loaded = copy_len;

        /* Load first byte and let ISR do the rest */
        UCB0TXBUF = tx_fifo[tx_count];
        UCB0IE |= UCTXIE;


        return copy_len; /* return number of bytes loaded into FIFO */
    }
}


/* clang-format off */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#else
#error Compiler not supported!
#endif
void USCI_B0_ISR(void)
/* clang-format on */
{
    /* If receive interrupt is pending*/
    if (UCB0IV & 0X02)
    {
        *rx_inptr = UCB0RXBUF;

        /* If we've received the delimiter, signal application */
        if (*rx_inptr == SPI_DELIM_CHAR)
        {
            rx_complete_signal = SPI_SIGNAL_SET;
        }

        /* advance in-pointer for next incoming byte */
        rx_inptr++;
        if (rx_inptr == &rx_ringbuf[sizeof(rx_ringbuf) - 1])
        {
            rx_inptr = rx_ringbuf;
        }
    }

    /* TX interrupt. indicates when TXBUF can be written */
    else if (UCB0IV & 0X04)
    {
        if (!(UCB0STAT & UCBUSY))
        {
            if (tx_count < tx_bytes_loaded)
            {
                if (UCB0STAT & UCOE)
                {
                    /* Transmit the missed byte if previous transmission overran
                     */
                    UCB0TXBUF = tx_fifo[tx_count];
                }
                else
                {
                    /* Transmit the next byte */
                    UCB0TXBUF = tx_fifo[++tx_count];
                }
            }
            else
            {
                tx_complete_signal = SPI_SIGNAL_SET;
                tx_bytes_loaded    = 0;
                tx_count           = 0;
                UCB0IE &= ~UCTXIE;
            }
        }
    }
}
#endif /* #if defined(TARGET_MCU) */

/**
 * @file spi.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-05
 *
 * @copyright Copyright (c) 2020 LORRIS ADCS
 *
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* memcpy */

#include "targets.h"

#include <msp430.h>
#include "spi.h"

static receive_func spi_rx_cb;


static volatile int spi_TX_count;
static int          spi_TX_bytes_loaded;
static char         spi_TX_buf[250];
static volatile int spi_TX_ready = 1;


void SPI0_init(receive_func rx)
{
    spi_rx_cb = rx;

    UCB0CTL1 |= UCSWRST; /* unlock ie: "reset" peripheral */

    /* Configure control registers */
    UCB0CTL0 |= UCMST;    /* master mode */
    UCB0CTL0 |= UCMODE_0; /* mode 0 (3 PIN SPI)*/
    UCB0CTL0 |= UCSYNC;   /* Synchronous mode (transmit clock) */

    UCB0CTL1 |= UCSSEL__SMCLK; /* Select SMclk (1MHz) to drive peripheral  */

    /* Configure bitrate registers */
    UCB0BR1 = 0x00;
    UCB0BR0 = 0x01;

    /* Re-enable peripheral */
    UCB0CTL1 &= ~UCSWRST;

    /* Configure alternate pin modes */
    P3SEL |= BIT0; /* P3.0 will be used for MOSI */
    P3SEL |= BIT1; /* P3.1 will be used for MISO */
    P3SEL |= BIT2; /* P3.2 will be used for SPICLK */

    /* Configure pin directions */
    P3DIR |= BIT0;  /* set MOSI pin to output mode */
    P3DIR &= ~BIT1; /* set MISO pin to input mode */
    P3DIR |= BIT2;  /* set SPICLK pin to output mode */
    P2DIR |= BIT3;  /* set CS pin to output mode */

    P2DIR &= ~BIT3; /* set CS_other pin low to select chip */
    P3OUT |= BIT1;

    /* Enable receive complete interrupt */
    UCB0IE |= UCRXIE;

    log_trace("initialized SPI on UCB0\n");
}

void SPI0_deinit(void)
{
    /* Disable interrupts */
    UCB0IE &= ~(UCRXIE | UCTXIE);

    /* Clear pending interrupt flags */
    UCB0IFG &= ~(UCTXIFG | UCRXIFG);

    spi_rx_cb = NULL;
    log_trace("deitialized SPI on UCB0\n");
}


int SPI0_transmit_IT(uint8_t *bytes, uint16_t len)
{
    if (spi_TX_ready && !(UCB0STAT & UCBUSY))
    {
        uint_fast16_t copy_len = len;
        if (len > sizeof(spi_TX_buf) / sizeof(*spi_TX_buf))
        {
            copy_len = sizeof(spi_TX_buf) / sizeof(*spi_TX_buf);
        }

        memcpy(spi_TX_buf, bytes, copy_len); /** @todo TRANSFER VIA DMA */

        spi_TX_count        = 0;
        spi_TX_bytes_loaded = copy_len;

        /* Load first byte into hardware, enable interrupts, let
         * ISR do the rest */
        spi_TX_ready = 0;
        UCB0TXBUF    = spi_TX_buf[spi_TX_count];

        UCB0IE |= UCTXIE;

        return 0;
    }
    return -1;
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    uint16_t flags = *(uint16_t *)&UCB0IV;

    /* If receive interrupt is pending*/
    if ((flags & UCRXIFG) == UCRXIFG)
    {
        if (!((UCB0STAT & UCBUSY) == UCBUSY))
        {
            if (NULL != spi_rx_cb)
            {
                spi_rx_cb(UCB0RXBUF);
            }
        }
    }

    /* TX interrupt. indicates when TXBUF can be written */
    else if ((flags & UCTXIFG) == UCTXIFG)
    {
        if (!((UCB0STAT & UCBUSY) == UCBUSY))
        {
            if (spi_TX_count < spi_TX_bytes_loaded)
            {
#if defined(SPI0_CATCH_OVERRUN)
                if ((UCB0STAT & UCOE) == UCOE) /* overrun error */
                {
                    /* Transmit the missed byte if previous transmission overran
                     */
                    UCB0TXBUF = spi_TX_buf[spi_TX_count];
                }
                else
                {
                    /* Transmit the next byte */
                    UCB0TXBUF = spi_TX_buf[++spi_TX_count];
                }
#else
                UCB0TXBUF = spi_TX_buf[++spi_TX_count];
#endif /* CATCH_OVERRUN */
            }
            else
            {
                spi_TX_bytes_loaded = 0;
                spi_TX_count        = 0;
                spi_TX_ready        = 1;

                /* Disable future SPI tx complete interrupts */
                UCB0IE &= ~UCTXIE;
            }
        }
    }
}
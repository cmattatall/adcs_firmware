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

static void transmit_byte_INTERNAL(uint8_t byte);

void SPI0_init(receive_func rx, SPI_DIR_t dir, SPI_MODE_t mode)
{
    spi_rx_cb = rx;

    UCB0CTL1 |= UCSWRST; /* unlock ie: "reset" peripheral */

    /* Configure control registers */
    UCB0CTL0 |= UCMST;    /* master mode */
    UCB0CTL0 |= UCMODE_0; /* mode 0 (3 PIN SPI)*/

    if (dir == SPI_DIR_msb)
    {
        UCB0CTL0 |= UCMSB;
    }
    else
    {
        UCB0CTL0 &= ~UCMSB;
    }

    if (mode == SPI_MODE_sync)
    {
        UCB0CTL0 |= UCSYNC;
    }
    else
    {
        UCB0CTL0 &= ~UCSYNC;
    }

    /* Explicitly disable loopback mode */
    UCB0STAT &= ~UCLISTEN;

    UCB0CTL1 |= UCSSEL__SMCLK; /* Select SMclk (1MHz) to drive peripheral  */

    /* Configure bitrate registers */
    UCB0BR0 |= 0x00;
    UCB0BR1 |= 0x04;

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

    UCB0IE |= UCRXIE; /* Enable receive interrupt */

    log_trace("initialized SPI on UCB0\n");
}

void SPI0_deinit(void)
{

    UCB0IE &= ~(UCRXIE | UCTXIE);    /* Disable interrupts */
    UCB0IFG &= ~(UCTXIFG | UCRXIFG); /* Clear pending interrupt flags */
    spi_rx_cb = NULL;                /* Reset callback */
    log_trace("deitialized SPI on UCB0\n");
}


int SPI0_transmit(uint8_t *bytes, uint16_t len)
{
    CONFIG_ASSERT(bytes != NULL);
    if (bytes != NULL)
    {
        unsigned int i;
        for (i = 0; i < len; i++)
        {
            transmit_byte_INTERNAL(bytes[i]);
        }
        return 0;
    }
    return -1;
}


void SPI0_transmit_byte(uint8_t byte)
{
    transmit_byte_INTERNAL(byte);
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    switch (__even_in_range(UCB0IV, 4))
    {
        case 0x02: /* receive interrupt pending */
        {
            if (NULL != spi_rx_cb)
            {
                spi_rx_cb(UCB0RXBUF);
            }
        }
        break;
    }
}


static void transmit_byte_INTERNAL(uint8_t byte)
{
    while (!(UCB0IFG & UCTXIFG))
    {
        /* Wait until TX buffer is ready again
         * (it should ideally already be) */
    }
    UCB0TXBUF = byte;
}
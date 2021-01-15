/**
 * @file spi.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for the SPI periphal driver on MSP430F5529
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


typedef enum
{
    SPI_TRANSMIT_STATUS_ok,
    SPI_TRANSMIT_STATUS_error,
    SPI_TRANSMIT_STATUS_empty,
    SPI_TRANSMIT_STATUS_timeout,
} SPI_TRANSMIT_STATUS_t;

typedef enum
{
    SPI_IRQ_rx,
    SPI_IRQ_tx,
} SPI_IRQ_t;

static receive_func          SPI0_rx_callback = NULL;
static volatile unsigned int tx_flag;

static volatile const uint8_t *SPI0_tx_ptr;
static volatile const uint8_t *SPI0_tx_ptr_end;

static void SPI0_enable_rx_irq(void);
static void SPI0_disable_rx_irq(void);


void SPI0_init(SPI_DIR_t dir, SPI_MODE_t mode)
{

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

    SPI0_enable_rx_irq();
    log_trace("initialized SPI on UCB0\n");
}

void SPI0_deinit(void)
{

    UCB0IE &= ~(UCRXIE | UCTXIE);    /* Disable interrupts */
    UCB0IFG &= ~(UCTXIFG | UCRXIFG); /* Clear pending interrupt flags */
    SPI0_rx_callback = NULL;         /* Reset callback */
    SPI0_disable_rx_irq();
    log_trace("deitialized SPI on UCB0\n");
}


void SPI0_set_receive_callback(receive_func rx)
{
    SPI0_rx_callback = rx;
}


int SPI0_transmit(const uint8_t *bytes, uint16_t len, void (*tx_callback)(void))
{
    int status = -1;
    CONFIG_ASSERT(bytes != NULL);
    if (len > 0)
    {
        SPI0_tx_ptr     = bytes;
        SPI0_tx_ptr_end = bytes + (len - 1);

        unsigned int i;
        tx_flag = 1;
        for (i = 0; i < len;)
        {
            if (tx_flag)
            {
                tx_flag = 0;
                while (UCB0STAT & UCBBUSY)
                {
                    /* Wait for bus to become available */
                }

                /* Transmit next byte */
                UCB0TXBUF = bytes[i];
                if (tx_callback != NULL)
                {
                    tx_callback();
                }

                i++;
            }
        }

        /* Check if all bytes transmitted */
        if (i == len)
        {
            status = 0;
        }
    }
    return status;
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    switch (__even_in_range(UCB0IV, 4))
    {
        case 0x02: /* Receive interrupt triggered IRQ */
        {
            if (NULL != SPI0_rx_callback)
            {
                // SPI0_rx_callback(UCB0RXBUF);
            }

            if ((UCB0IFG & UCTXIFG) == UCTXIFG)
            {
                tx_flag = 1;
            }
        }
        break;
    }
}


static void SPI0_enable_rx_irq(void)
{
    UCB0IE |= UCRXIE;
}


static void SPI0_disable_rx_irq(void)
{
    UCB0IE &= ~UCRXIE;
}
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

#define UCB0IVNOP (0x00)
#define UCB0IVRX (0x02)
#define UCB0IVTX (0x04)

#define UCB0_SPI0_MOSI_PIN (BIT0)
#define UCB0_SPI0_MISO_PIN (BIT1)
#define UCB0_SPI0_CLK_PIN (BIT2)

#define UCB0_SPI0_MOSI_SEL (P3SEL)
#define UCB0_SPI0_MISO_SEL (P3SEL)
#define UCB0_SPI0_CLK_SEL (P3SEL)

#define UCB0_SPI0_MOSI_DIR (P3DIR)
#define UCB0_SPI0_MISO_DIR (P3DIR)
#define UCB0_SPI0_CLK_DIR (P3DIR)

#define UCB0_SPI0_MOSI_OUT (P3OUT)
#define UCB0_SPI0_MISO_OUT (P3OUT)
#define UCB0_SPI0_CLK_OUT (P3OUT)

#define UCB0_SPI0_MOSI_REN (P3REN)
#define UCB0_SPI0_MISO_REN (P3REN)
#define UCB0_SPI0_CLK_REN (P3REN)

typedef enum
{
    SPI_IRQ_rx,
    SPI_IRQ_tx,
} SPI_IRQ_t;


static receive_func SPI0_rx_callback = NULL;
static void (*SPI0_tx_cplt_callback)(void);

static void SPI0_PHY_config(void);

void SPI0_init(receive_func rx, const SPI_init_struct *init)
{

    UCB0CTL1 |= UCSWRST; /* unlock peripheral to modify config */

    if (init->role == SPI_ROLE_master)
    {
        UCB0CTL0 |= UCMST;
    }
    else
    {
        UCB0CTL0 &= ~UCMST;
    }

    if (init->phy_cfg == SPI_PHY_3)
    {
        UCB0CTL0 &= ~(UCMODE_3 | UCMODE_2 | UCMODE_1 | UCMODE_0);
        UCB0CTL0 |= UCMODE_0; /* mode 0 (3 PIN SPI)*/
    }
    else
    {
        UCB0CTL0 &= ~(UCMODE_3 | UCMODE_2 | UCMODE_1 | UCMODE_0);
        UCB0CTL0 |= UCMODE_1; /* mode 0 (3 PIN SPI)*/
    }


    if (init->polarity == SPI_CLK_POLARITY_high)
    {
        UCB0CTL0 |= UCCKPL;
    }
    else
    {
        UCB0CTL0 &= ~UCCKPL;
    }


    if (init->edge_phase == SPI_DATA_CHANGE_edge2)
    {
        UCB0CTL0 |= UCCKPH;
    }
    else
    {
        UCB0CTL0 &= ~UCCKPH;
    }


    if (init->data_dir == SPI_DATA_DIR_msb)
    {
        UCB0CTL0 |= UCMSB; /* MSB first */
    }
    else
    {
        UCB0CTL0 &= ~UCMSB; /* LSB first */
    }


    if (init->tim_mode == SPI_TIM_MODE_sync)
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


    UCB0BR0 = 0x0F;
    UCB0BR1 = 0x00;

    UCB0CTL1 &= ~UCSWRST;

    SPI0_PHY_config();

    /* Reset transmit configuration */
    SPI0_tx_cplt_callback = NULL;
    UCB0IE &= ~UCTXIE;

    /* Register IRQ service cb from caller */
    SPI0_rx_callback = rx;
}


void SPI0_deinit(void)
{
    /* Disable interrupts */
    UCB0IE &= ~UCTXIE;
    UCB0IE &= ~UCRXIE;


    /* Clear pending interrupt flags */
    UCB0IFG &= ~(UCTXIFG | UCRXIFG);

    /* Reset callback */
    SPI0_rx_callback = NULL;

    /* Reset transmit configuration */
    SPI0_tx_cplt_callback = NULL;
}


void SPI0_enable_rx_irq(void)
{
    UCB0IE |= UCRXIE;
    _no_operation(); /* Fix for garbage silicon erratta */
}


void SPI0_disable_rx_irq(void)
{
    UCB0IE &= ~UCRXIE;
}

static volatile const uint8_t *SPI0_tx_ptr;
static volatile const uint8_t *SPI0_tx_ptr_end;

static void SPI0_transmit_byte(void)
{
    uint8_t byte = *SPI0_tx_ptr;
    SPI0_tx_ptr++;
    if (SPI0_tx_ptr == SPI0_tx_ptr_end)
    {
        SPI0_tx_ptr_end       = NULL;
        SPI0_tx_ptr           = NULL;
        SPI0_tx_cplt_callback = NULL;
        UCB0IE &= ~UCTXIE;
    }
    UCB0TXBUF = byte;
    while ((UCB0IFG & UCTXIFG) != UCTXIFG)
    {
        /* Wait for tx shift register to flush */
    }
}

int SPI0_transmit(const uint8_t *bytes, uint16_t len, void (*tx_cb)(void))
{
    CONFIG_ASSERT(bytes != NULL);
    CONFIG_ASSERT(len > 0);
    if (SPI0_tx_ptr == NULL)
    {
        SPI0_tx_ptr           = bytes;
        SPI0_tx_ptr_end       = bytes + len;
        SPI0_tx_cplt_callback = tx_cb;
        SPI0_transmit_byte();
        if (len > 1)
        {
            UCB0IE |= UCTXIE; /* IRQ will handle subsequent bytes */
        }
        return 0;
    }
    else
    {
        return -1;
    }
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    switch (UCB0IV)
    {
        case UCB0IVRX:
        {
            if (NULL != SPI0_rx_callback)
            {
                SPI0_rx_callback(UCB0RXBUF);
            }
        }
        break;
        case UCB0IVTX:
        {
            if (SPI0_tx_cplt_callback != NULL)
            {
                SPI0_tx_cplt_callback();
            }

            if (SPI0_tx_ptr != NULL)
            {
                SPI0_transmit_byte();
            }
        }
        break;
    }
}


static void SPI0_PHY_config(void)
{
    /* Configure alternate pin mode */
    UCB0_SPI0_MOSI_SEL |= UCB0_SPI0_MOSI_PIN;
    UCB0_SPI0_MISO_SEL |= UCB0_SPI0_MISO_PIN;
    UCB0_SPI0_CLK_SEL |= UCB0_SPI0_CLK_PIN;

    /* Configure pin directions */
    UCB0_SPI0_MOSI_DIR |= UCB0_SPI0_MOSI_PIN;  /* set MOSI pin to output mode */
    UCB0_SPI0_MISO_DIR &= ~UCB0_SPI0_MISO_PIN; /* set MISO pin to input mode */
    UCB0_SPI0_CLK_DIR |= UCB0_SPI0_CLK_PIN; /* set SPICLK pin to output mode */

    UCB0_SPI0_MISO_OUT &= ~UCB0_SPI0_MISO_PIN; /* Pulldown MISO */
    UCB0_SPI0_MISO_REN &= ~UCB0_SPI0_MISO_PIN; /* Disable MISO pullup res */
}
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

typedef struct
{
    volatile const uint8_t *ptr;
    volatile const uint8_t *ptr_end;
    void (*cplt_callback)(void);
} tx_config;


static receive_func SPI0_rx_callback = NULL;
static tx_config    SPI0_tx_cfg;

static void SPI0_transmit_byte(void);
static void SPI0_enable_tx_irq(void);
static void SPI0_disable_tx_irq(void);
static void SPI0_reset_tx_config(tx_config *cfg);
static void SPI0_PHY_config(void);


void SPI0_init(receive_func rx, SPI_DATA_DIR_t dir, SPI_MODE_t mode)
{

    UCB0CTL1 |= UCSWRST; /* unlock peripheral to modify config */

    UCB0CTL0 |= UCMST;    /* master mode */
    UCB0CTL0 |= UCMODE_0; /* mode 0 (3 PIN SPI)*/

    UCB0CTL0 |= UCCKPH;

    if (dir == SPI_DATA_DIR_msb)
    {
        UCB0CTL0 |= UCMSB; /* MSB first */
    }
    else
    {
        UCB0CTL0 &= ~UCMSB; /* LSB first */
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

    /* bitrate registers */
    UCB0BR0 |= 0x00;
    UCB0BR1 |= 0x04;

    UCB0CTL1 &= ~UCSWRST;

    SPI0_PHY_config();

    /* Reset transmit configuration */
    SPI0_reset_tx_config(&SPI0_tx_cfg);

    /* Register IRQ service cb from caller */
    SPI0_rx_callback = rx;
}


void SPI0_deinit(void)
{
    /* Disable interrupts */
    SPI0_disable_rx_irq();
    SPI0_disable_tx_irq();

    /* Clear pending interrupt flags */
    UCB0IFG &= ~(UCTXIFG | UCRXIFG);

    /* Reset callback */
    SPI0_rx_callback = NULL;

    /* Reset transmit configuration */
    SPI0_tx_cfg.ptr           = NULL;
    SPI0_tx_cfg.ptr_end       = NULL;
    SPI0_tx_cfg.cplt_callback = NULL;
}


void SPI0_enable_rx_irq(void)
{
    UCB0IE |= UCRXIE;
    _no_operation(); /* See page 60 of user guide! */
}


void SPI0_disable_rx_irq(void)
{
    UCB0IE &= ~UCRXIE;
    _no_operation(); /* See page 60 of user guide! */
}


int SPI0_transmit(const uint8_t *bytes, uint16_t len, void (*tx_cb)(void))
{
    CONFIG_ASSERT(bytes != NULL);
    if (SPI0_tx_cfg.ptr == NULL)
    {
        if (len > 0)
        {
            SPI0_tx_cfg.cplt_callback = tx_cb;
            SPI0_tx_cfg.ptr           = bytes;
            SPI0_tx_cfg.ptr_end       = bytes + len;
            SPI0_transmit_byte();
            SPI0_enable_tx_irq();
        }

        return 0;
    }
    return -1;
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    switch (UCB0IV)
    {
        case UCB0IVNOP:
        {
            /* No interrupt pending */
        }
        break;
        case UCB0IVRX: /* Receive interrupt triggered IRQ */
        {
            if (NULL != SPI0_rx_callback)
            {
                SPI0_rx_callback(UCB0RXBUF);
            }
        }
        break;
        case UCB0IVTX:
        {
            /** @todo IF we've hardened the driver
             * logic in both release and
             * debug modes, we can remove this check
             */
            if (SPI0_tx_cfg.ptr != NULL)
            {
                if (SPI0_tx_cfg.cplt_callback != NULL)
                {
                    SPI0_tx_cfg.cplt_callback();
                }
                SPI0_transmit_byte();
            }
        }
        break;
        default:
        {
            /* Do nothing */
        }
        break;
    }
}


static void SPI0_transmit_byte(void)
{
    /* Transmit next byte */
    uint8_t byte = *SPI0_tx_cfg.ptr;
    SPI0_tx_cfg.ptr++;
    if (SPI0_tx_cfg.ptr == SPI0_tx_cfg.ptr_end)
    {
        SPI0_reset_tx_config(&SPI0_tx_cfg);
    }
    UCB0TXBUF = byte;
}


static void SPI0_enable_tx_irq(void)
{
    UCB0IE |= UCTXIE;
    _no_operation(); /* See page 60 of user guide! */
}


static void SPI0_disable_tx_irq(void)
{
    UCB0IE &= ~UCTXIE;
    _no_operation(); /* See page 60 of user guide! */
}


static void SPI0_reset_tx_config(tx_config *cfg)
{
    cfg->cplt_callback = NULL;
    cfg->ptr           = NULL;
    cfg->ptr_end       = NULL;
    SPI0_disable_tx_irq();
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
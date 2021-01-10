/**
 * @file sun_sensor.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example executable to demonstrate transmission via SPI
 * @version 0.1
 * @date 2021-01-08
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 */

#include <msp430.h>
#include <stdint.h>
#include <string.h>

static volatile int timer_expired;
static volatile int spi_RX_complete;

static volatile char spi_RX_buf[250];
static volatile int  spi_RX_idx;

static volatile int spi_TX_count;
static int          spi_TX_bytes_loaded;
static char         spi_TX_buf[250];
static volatile int spi_TX_ready = 1;

static void UCB0_SPI_init(void);
static int  SPI0_transmit_IT(uint8_t *bytes, uint16_t len);
static void example_init(void);


int main(void)
{
    example_init();
    UCB0_SPI_init();

    uint8_t  msg[]   = "Hello World!!";
    uint16_t msg_len = sizeof(msg);
    while (1)
    {
        if (timer_expired)
        {
            P1OUT ^= 0x01;
            timer_expired = 0;
        }

        SPI0_transmit_IT(msg, msg_len);


        if (spi_RX_complete)
        {
            /* Do stuff with the data */


            spi_RX_complete = 0;
        }
    }
}


__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    timer_expired = 1;
}


static void UCB0_SPI_init(void)
{
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
}

static void example_init(void)
{
    WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
    P1DIR |= 0x01;            /* Set P1.0 to output direction */

    /* Configure timer A0 to generate compare interrupt ~ once per sec */
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */
    TA0CCR0  = 50000;

    _BIS_SR(GIE); /* enable interrupts */
}


static int SPI0_transmit_IT(uint8_t *bytes, uint16_t len)
{
    if (spi_TX_ready)
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
            spi_RX_buf[spi_RX_idx] = UCB0RXBUF;
            spi_RX_idx++;

            if (spi_RX_idx > (int)(sizeof(spi_RX_buf) / sizeof(*spi_RX_buf)))
            {
                spi_RX_complete = 1;
                spi_RX_idx      = 0;
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

#if defined(CATCH_OVERRUN)
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
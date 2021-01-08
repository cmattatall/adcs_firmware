/**
 * @file sun_sensor.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example executable to demonstrate reading from the sun sensor.
 *        Collects data via spi on UCB0 line.
 * @version 0.1
 * @date 2021-01-08
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @note
 * PINOUT:
 ***************************************
 *   MSP430                   SUN_SENSOR
 *   5V --------------------------- 5V
 *
 *   GND -------------------------- GND
 *
 *   P3.0 (UCB0 MOSI) ------------- MOSI
 *
 *   P3.1 (UCB0 MISO) ------------- MISO
 *
 *   P3.2 (UCB0CLK) --------------- CLK
 *
 *   P2.3 (SPICS_other)-------------CS
 *
 *   NC                             3V3
 ****************************************
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
static volatile int spi_TX_complete;


static void stop_watchdog(void);
static void red_led_init(void);
static void enable_interrupts(void);
static void TIMERA0_init(void);
static void UCB0_SPI_init(void);


int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    UCB0_SPI_init();
    enable_interrupts();
    while (1)
    {
        if (timer_expired)
        {
            P1OUT ^= 0x01;
            timer_expired = 0;
        }


        if (spi_TX_complete)
        {
        }


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


static void stop_watchdog(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
}


static void red_led_init(void)
{
    P1DIR |= 0x01; // Set P1.0 to output direction
}

static void enable_interrupts(void)
{
    _BIS_SR(GIE); // Enter LPM0 w/ interrupt
}


static void TIMERA0_init(void)
{
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */
    TA0CCR0  = 50000;
}


static void UCB0_SPI_init(void)
{
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

    UCB0STAT &= ~UCLISTEN;

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
}


unsigned int SPI0_transmit_IT(uint8_t *bytes, uint16_t len)
{
    if (spi_TX_complete)
    {
        spi_TX_complete = 0;

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
        UCB0TXBUF = spi_TX_buf[spi_TX_count];
        UCB0IE |= UCTXIE;

        return copy_len; /* return number of bytes loaded into FIFO */
    }
    return 0;
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_B0_VECTOR_ISR(void)
{
    /* If receive interrupt is pending*/
    if (UCB0IV & 0X02)
    {
        spi_RX_buf[spi_RX_idx] = UCB0RXBUF;
        spi_RX_idx++;

        if (spi_RX_idx > sizeof(spi_RX_buf) / sizeof(*spi_RX_buf))
        {
            spi_RX_complete = 1;
            spi_RX_idx      = 0;
        }
    }

    /* TX interrupt. indicates when TXBUF can be written */
    else if (UCB0IV & 0X04)
    {
        if (!(UCB0STAT & UCBUSY))
        {
            if (spi_TX_count < spi_TX_bytes_loaded)
            {
                if (UCB0STAT & UCOE)
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
            }
            else
            {
                spi_TX_bytes_loaded = 0;
                spi_TX_count        = 0;
                spi_TX_complete     = 1;

                /* Disable future SPI tx complete interrupts */
                UCB0IE &= ~UCTXIE;
            }
        }
    }
}
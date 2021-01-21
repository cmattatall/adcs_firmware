/**
 * @file spi_example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief File to demonstrate usage of USCI_B SPI0 peripheral API
 * @version 0.1
 * @date 2021-01-21
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <msp430.h>
#include "spi.h"

static volatile int timer_expired = 0;

static void stop_watchdog(void);
static void red_led_init(void);
static void enable_interrupts(void);
static void TIMERA0_init(void);

static volatile uint8_t rx_buf[20];
static volatile uint8_t rx_idx;

static void my_SPI_rx_callback(uint8_t byte_from_spi_rx_reg)
{
    rx_buf[rx_idx] = byte_from_spi_rx_reg;
    rx_idx++;
    if(rx_idx > sizeof(rx_buf))
    {
        rx_idx = 0;
    }
}


int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    
    SPI_init_struct init;
    init.role     = SPI_ROLE_master;
    init.phy_cfg  = SPI_PHY_3;
    init.data_dir = SPI_DATA_DIR_msb;
    init.edge_phase = SPI_DATA_CHANGE_edge1;
    init.polarity   = SPI_CLK_POLARITY_low;

    uint16_t prescaler = 0x0002;

    SPI0_init(my_SPI_rx_callback, &init, prescaler);
    SPI0_enable_rx_irq();

    enable_interrupts();


    uint8_t transmit_msg[] = "Hello World\r\n";
    while (1)
    {
        if (timer_expired)
        {
            P1OUT ^= 0x01;
            timer_expired = 0;

            SPI0_transmit(transmit_msg, sizeof(transmit_msg));
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

    /* 1000000 * (1/8) * (1/8) == 15625 */
    /* If we want to blink once per second, we need to interrupt twice per sec*/
    /* (because toggle) */
    /* thus, TA0CCR0 should be 15625/2 == 7812 if we want to blink once per
     * sec*/
    /*
     * BUT, FOR SOME REASON, WHEN I DO THIS, THE DAMN THING BLINKS LIKE 13
     * TIMES A SECOND....
     *
     * SO EITHER THE DATASHEET DOCUMENTATION ON FREQUENCY OF SMCLK IS WRONG, OR
     * SOMETHING ELSE IS GOING ON...
     *
     * TIMERA IS IN SECTION 17.2 OF USER MANUAL
     * https://www.ti.com/lit/ug/slau208q/slau208q.pdf?ts=1608390374754&ref_url=https%253A%252F%252Fwww.google.com%252F
     *
     * for now, just setting the compare value to 50k so we get a heartbeat
     * blink. The entire purpose of the heartbeat blink is to have a visual
     * indication of lockup (since if a chip hardfaults the debugger would
     * also become nonresponsive)
     */
    TA0CCR0 = 50000;
}
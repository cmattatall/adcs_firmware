/**
 * @file ads7841_example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example executable to demonstrate usage of ads7841 driver API
 * @version 0.1
 * @date 2021-01-09
 *
 * @copyright Copyright (c) 2021 DSS LORIS PROJECT
 *
 * @note
 * PINOUT:
 *  ***************************************
 *   MSP430                       ADS7841
 *   5V --------------------------- 5V
 *   GND -------------------------- GND
 *   P3.0 (UCB0 MOSI) ------------- MOSI
 *   P3.1 (UCB0 MISO) ------------- MISO
 *   P3.2 (UCB0CLK) --------------- CLK
 *   P2.3 (SPICS_other)-------------CS
 *   3.3V ------------------------- 3.3V
 *   3.3v ------------------------- CHANNEL_3
 ****************************************
 */
#include <msp430.h>

#include "ads7841e.h"

static volatile int timer_count = 0;

__interrupt_vec(TIMER0_A0_VECTOR) void Timer_A(void)
{
    timer_count++;
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
    _BIS_SR(GIE);    // Enter LPM0 w/ interrupt
    _no_operation(); /* Fixes bug due to silicon erratta */
}


static void TIMERA0_init(void)
{
    TA0CTL &= ~(ID0 | ID1);
    TA0CTL |= ID_3; /* input prescaler to 8 */
    TA0EX0 &= ~(TAIDEX0 | TAIDEX1 | TAIDEX2);
    TA0EX0 |= TAIDEX_7;           /* set expansion prescaler to 8 */
    TA0CCTL0 = CCIE;              /* CCR0 interrupt enabled */
    TA0CTL   = TASSEL_2 + MC__UP; /* source from SMCLK, count up to TA0CCR0 */
    TA0CCR0  = 1000;
}


static void ADS7841_chip_select_init(void)
{
    P2DIR |= BIT3; /* set CS pin to output mode */
}


static void ADS7841_chip_select_func(void)
{
    P2DIR &= ~BIT3; /* set CS_pin low to select chip */
}

static void ADS7841_chip_unselect_func(void)
{
    P2DIR |= BIT3; /* set CS_pin low to select chip */
}

int main(void)
{
    stop_watchdog();
    red_led_init();
    TIMERA0_init();
    ADS7841_chip_select_init();
    ADS7841_driver_init(ADS7841_chip_select_func, ADS7841_chip_unselect_func,
                        ADS7841_PWRMODE_always_on, ADS7841_CONVMODE_12);
    enable_interrupts();
    uint16_t val;
    while (1)
    {
        if (timer_count)
        {
            P1OUT ^= 0x01;

            val = ADS7841_measure_channel(ADS7841_CHANNEL_3);
            if (val != ADS7841_CONV_STATUS_BUSY)
            {
                /* Do stuff with the digitized analog value */
            }
            timer_count = 0;
        }
    }
}
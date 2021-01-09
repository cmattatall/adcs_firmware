/**
 * @file spi_api_example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to demonstrate usage of the SPI driver API
 * @version 0.1
 * @date 2021-01-09
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 * 
 * @note
 * PINOUT:
 * ***************************************
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
#include <stdint.h>
#include <msp430.h>

#include "spi.h"

static volatile unsigned int buffer_idx;
static volatile uint8_t      rx_buf[50];

static void enable_interrupts(void)
{
    _BIS_SR(GIE);
}

static void my_receive_func(uint8_t byte)
{
    rx_buf[buffer_idx] = byte;
    buffer_idx++;
    if (buffer_idx > sizeof(rx_buf) / sizeof(*rx_buf))
    {
        buffer_idx = 0;
    }
}

int main(void)
{
    SPI0_init(my_receive_func);
    enable_interrupts();

    uint8_t  msg[]  = "Hello";
    uint16_t msglen = sizeof(msg) / sizeof(*msg);

    while (1)
    {
        SPI0_transmit_IT(msg, msglen);
    }
}
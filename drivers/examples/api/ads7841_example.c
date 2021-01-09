/**
 * @file ads7841_example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Example executable to demonstrate usage of ads7841 driver API
 * @version 0.1
 * @date 2021-01-09
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 * @note
 * PINOUT:
 *  ***************************************
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

#include "ads7841e.h"

int main(void)
{
    ADS7841_driver_init();

    while (1)
    {
        uint16_t val = ADS7841_get_conv(ADS7841_CHANNEL_0, ADS7841_CONVTYPE_12);
    }
}
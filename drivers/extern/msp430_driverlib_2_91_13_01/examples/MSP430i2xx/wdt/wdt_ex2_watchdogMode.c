/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP430i2xx Watchdog Timer - Watchdog Mode
 *
 * Description: In this example, the Watchdog Timer is setup in watchdog mode
 * using SMCLK as a source. SMCLK is configured as the DCO / 16 and the WDT
 * is configured as SMCLK / 32768 resulting in a Watchdog period of 32ms. The
 * LED is turned on when entering LPM and turned off once the device has been
 * reset by the WDT. After reset the device checks to make sure it was the
 * WDT that caused the interrupt and turns off the LED.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |              P1.4|-->LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    // Stop the WDT
    WDT_hold(WDT_BASE);

    // Check if WDT caused reset
    if(SFR_getInterruptStatus(SFR_WATCHDOG_INTERRUPT)) {
        SFR_clearInterrupt(SFR_WATCHDOG_INTERRUPT);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    }

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Setup DCO to use internal resistor. DCO calibrated at 16.384 MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Setup SMCLK as DCO / 16 = 1.024 MHz
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_16);

    // Configure interval to SMCLK / 32768 = 32ms
    WDT_initWatchdogTimer(WDT_BASE,
                          WDT_CLOCKSOURCE_SMCLK,
                          WDT_CLOCKDIVIDER_32K);
    WDT_resetTimer(WDT_BASE);

    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);

    // Start the WDT and enter LPM0
    WDT_start(WDT_BASE);
    __bis_SR_register(LPM0_bits);
}

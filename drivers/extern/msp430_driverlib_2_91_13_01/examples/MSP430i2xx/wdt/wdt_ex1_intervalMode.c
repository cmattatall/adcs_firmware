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
 * MSP430i2xx Watchdog Timer - Interval Mode
 *
 * Description: In this example, the Watchdog Timer is setup in interval with
 * ACLK as a source. The WDT divides ACLK by 32768 so that it interrupts in
 * 1s intervals. The LED is toggled in the WDT ISR to show that the ISR
 * has been reached.
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

    // Setup DCO to use internal resistor. DCO calibrated at 16.384 MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Configure interval to ACLK / 32768 = 1s
    WDT_initIntervalTimer(WDT_BASE,
                          WDT_CLOCKSOURCE_ACLK,
                          WDT_CLOCKDIVIDER_32K);
    WDT_resetTimer(WDT_BASE);

    // LED for monitoring interval
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Start the WDT and enter LPM0 with interrupts enabled
    SFR_clearInterrupt(SFR_WATCHDOG_INTERRUPT);
    SFR_enableInterrupt(SFR_WATCHDOG_INTERRUPT);
    WDT_start(WDT_BASE);
    __bis_SR_register(LPM0_bits | GIE);
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(WDT_VECTOR)))
#endif
void WDT_ISR(void) {
    // Toggle LED
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
}

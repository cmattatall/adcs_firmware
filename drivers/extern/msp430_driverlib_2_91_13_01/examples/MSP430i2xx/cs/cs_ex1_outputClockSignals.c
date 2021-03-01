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
 * MSP430i2xx Clock System - Output Clock Signals
 *
 * Description: In this example, the DCO is setup to use the internal resistor.
 * Then the GPIO pins are configured in peripheral function mode so that the
 * clock signals are output on the pins. Finally, clock dividers are set for
 * the MCLK and SMCLK to show a divided clock. The divided clocks can be seen
 * on the pins using an measurement device. This example demonstrates how to
 * setup the DCO and setup the clock speed for the device.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |              P1.0|-->MCLK = DCO/2 = 8.192MHz
 *          --|RST               |
 *            |              P1.1|-->SMCLK = DCO/16 = 1.024MHz
 *            |                  |
 *            |              P1.2|-->ACLK = DCO/512 = 32kHz
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    WDT_hold(WDT_BASE);

    // Setting the DCO to use the internal resistor. DCO will be at 16.384MHz
    CS_setupDCO(CS_INTERNAL_RESISTOR);

    // Set P1.0, P1.1, P1.2 to output clock signals
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                                GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2,
                                                GPIO_SECONDARY_MODULE_FUNCTION);

    // Setting dividers for MCLK and SMCLK
    CS_initClockSignal(CS_MCLK, CS_CLOCK_DIVIDER_2);
    CS_initClockSignal(CS_SMCLK, CS_CLOCK_DIVIDER_16);

    // Stay in active mode so all clocks keep running
    while(1) {

    }

}

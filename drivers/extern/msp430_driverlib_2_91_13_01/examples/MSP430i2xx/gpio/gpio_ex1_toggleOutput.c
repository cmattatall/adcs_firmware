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
 * MSP430i2xx GPIO - Toggle Output
 *
 * Description: In this example, P1.4 is setup as an output pin. The pin is
 * toggled in the main loop to make the LED blink. The MCLK is slowed down so
 * the LED blinking is visible.
 *
 *
 *                MSP430i2041
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |              P1.4|-->LED
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Zack Lalanne
 ******************************************************************************/

#include "driverlib.h"

int main(void) {

    WDT_hold(WDT_BASE);

    // Set P1.4 as output pin to drive the LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);

    // Slow down clock so we can see LED blink
    // Configure MCLK = ~1MHz
    CS_initClockSignal(CS_MCLK, CS_CLOCK_DIVIDER_16);

    while(1) {
        // Toggle the LED state
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
        __delay_cycles(100000);
    }
}
